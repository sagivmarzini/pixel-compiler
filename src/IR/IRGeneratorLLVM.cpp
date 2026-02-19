//
// Created by Sagiv Marzini on 14/01/2026.
//

#include "IRGeneratorLLVM.h"

#include "parse/AST/Expression.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <vector>

#include "parse/AST/Statement.h"
#include "semantic/Symbol.h"
#include "semantic/SymbolTable.h"

IRGeneratorLLVM::IRGeneratorLLVM() {
    _context = std::make_unique<llvm::LLVMContext>();
    _module  = std::make_unique<llvm::Module>("pixel_compiler", *_context);
    _builder = std::make_unique<llvm::IRBuilder<> >(*_context);
}

llvm::Value* IRGeneratorLLVM::visit(const Program& program) {
    for (const auto& stmt: program.statements) {
        stmt->acceptIR(*this);
    }
    // Return the module at the end for printing/saving
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const FunctionDeclaration& node) {
    using llvm::Function;

    std::vector<llvm::Type*> paramTypes;
    paramTypes.reserve(node.parameters.size());
    for (const auto& param: node.parameters) {
        paramTypes.push_back(compilerTypeToLlvmType(param.type));
    }

    const auto returnType = compilerTypeToLlvmType(node.returnType);

    // Return type and parameter types
    const auto functionType = llvm::FunctionType::get(returnType, paramTypes, false);

    const auto function = Function::Create(functionType, Function::ExternalLinkage, node.name, _module.get());

    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*_context, "entry", function);
    _builder->SetInsertPoint(basicBlock);

    int index = 0;
    for (auto& arg: function->args()) {
        const auto& param = node.parameters[index++];
        arg.setName(param.name);

        // Create an alloca for the parameter
        llvm::Type*       type   = arg.getType();
        llvm::AllocaInst* alloca = _builder->CreateAlloca(type, nullptr, std::string(arg.getName()) + "_addr");

        // Store the initial value (the argument) into the alloca
        _builder->CreateStore(&arg, alloca);

        // Map the name to the ALLOCA (the pointer), not the argument value
        _namedValues.emplace(param.symbol, alloca);
    }

    node.body->acceptIR(*this);
    if (!_builder->GetInsertBlock()->getTerminator()) {
        _builder->CreateRetVoid();
    }

    return function;
}

llvm::Value* IRGeneratorLLVM::visit(const FunctionCall& node) {
    const auto calledFunction = _module->getFunction(node.functionName);
    if (!calledFunction) {
        throw std::runtime_error("Function not found: " + node.functionName);
    }

    std::vector<llvm::Value*> args;
    for (const auto& arg: node.arguments) {
        args.push_back(arg.value->acceptIR(*this));
        if (!args.back())
            return nullptr;
    }

    return _builder->CreateCall(calledFunction, args);
}

llvm::Value* IRGeneratorLLVM::visit(const IfStatement& node) {
    const auto condition = node.condition->acceptIR(*this);

    const auto theFunction = _builder->GetInsertBlock()->getParent();

    // Create basic blocks for the `then` and `else` cases
    const auto thenBlock  = llvm::BasicBlock::Create(*_context, "then", theFunction);
    const auto elseBlock  = llvm::BasicBlock::Create(*_context, "else");
    const auto mergeBlock = llvm::BasicBlock::Create(*_context, "ifcont");

    _builder->CreateCondBr(condition, thenBlock, elseBlock);

    // Emit `then` value
    _builder->SetInsertPoint(thenBlock);
    node.thenBranch->acceptIR(*this);
    bool thenHasTerminator = _builder->GetInsertBlock()->getTerminator() != nullptr;
    if (!thenHasTerminator) {
        _builder->CreateBr(mergeBlock);
    }

    // Emit 'else' value
    elseBlock->insertInto(theFunction);
    _builder->SetInsertPoint(elseBlock);
    if (node.elseBranch) {
        node.elseBranch->acceptIR(*this);
    }
    bool elseHasTerminator = _builder->GetInsertBlock()->getTerminator() != nullptr;
    if (!elseHasTerminator) {
        _builder->CreateBr(mergeBlock);
    }

    // Only insert merge block if at least one branch needs it
    if (!thenHasTerminator || !elseHasTerminator) {
        mergeBlock->insertInto(theFunction);
        _builder->SetInsertPoint(mergeBlock);
    } else {
        // Both branches have terminators, merge block is unreachable
        // Don't insert it, and leave builder pointing to a terminated block
        delete mergeBlock;
    }

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const WhileLoop& node) {
    const auto theFunction = _builder->GetInsertBlock()->getParent();

    // Create the blocks
    const auto condBB  = llvm::BasicBlock::Create(*_context, "whilecond", theFunction);
    const auto bodyBB  = llvm::BasicBlock::Create(*_context, "whilebody", theFunction);
    const auto afterBB = llvm::BasicBlock::Create(*_context, "whileafter");

    // Jump from the current location to the condition check
    _builder->CreateBr(condBB);

    // Generate the condition
    _builder->SetInsertPoint(condBB);
    const auto condV = node.condition->acceptIR(*this);
    _builder->CreateCondBr(condV, bodyBB, afterBB);

    // Generate the loop body
    _builder->SetInsertPoint(bodyBB);
    node.body->acceptIR(*this);

    // Only loop back if the body didn't already 'return'
    if (!_builder->GetInsertBlock()->getTerminator()) {
        _builder->CreateBr(condBB);
    }

    // Exit: Carry on with the rest of the program
    afterBB->insertInto(theFunction);
    _builder->SetInsertPoint(afterBB);

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const ForLoop& node) {
    llvm::Value* startVal = node.range->start->acceptIR(*this);
    if (!startVal) return nullptr;

    llvm::Value* variableAddr = initLocalVariable(startVal->getType(), node.identifier, startVal);
    _namedValues[node.symbol] = variableAddr;

    llvm::Function*   theFunction = _builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB      = llvm::BasicBlock::Create(*_context, "loop", theFunction);
    llvm::BasicBlock* afterBB     = llvm::BasicBlock::Create(*_context, "afterloop", theFunction);

    // Fall through from current block to loop start
    _builder->CreateBr(loopBB);
    _builder->SetInsertPoint(loopBB);

    node.body->acceptIR(*this);

    // Only continue with increment and condition if body didn't terminate
    if (!_builder->GetInsertBlock()->getTerminator()) {
        // Increment: Load, Add, Store
        llvm::Value* stepVal    = node.step->acceptIR(*this);
        llvm::Value* currentVal = _builder->CreateLoad(compilerTypeToLlvmType(node.range->type), variableAddr,
                                                       node.identifier);

        llvm::Value* nextVal;
        llvm::Type*  loopVarType = currentVal->getType();

        // Check if we're dealing with floats or ints
        if (loopVarType->isFloatingPointTy()) {
            nextVal = _builder->CreateFAdd(currentVal, stepVal, "nextvar");
        } else {
            nextVal = _builder->CreateAdd(currentVal, stepVal, "nextvar");
        }
        _builder->CreateStore(nextVal, variableAddr);

        llvm::Value* endCond = node.range->end->acceptIR(*this);

        // Compare current loop var against end condition
        llvm::Value* loopCond;
        if (loopVarType->isFloatingPointTy()) {
            // For floats, use ordered less than (OLT)
            loopCond = _builder->CreateFCmpOLT(nextVal, endCond, "loopcond");
        } else {
            // For ints, use signed less than (SLT)
            loopCond = _builder->CreateICmpSLT(nextVal, endCond, "loopcond");
        }

        // Conditional branch: if loopCond is true, go to loopBB, else afterBB
        _builder->CreateCondBr(loopCond, loopBB, afterBB);
    }

    _builder->SetInsertPoint(afterBB);

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const RangeExpression& node) {
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const Block& node) {
    for (const auto& stmt: node.statements) {
        stmt->acceptIR(*this);
    }
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const VariableDeclaration& node) {
    const auto symbol = node.symbol;
    const auto type   = compilerTypeToLlvmType(symbol->type);

    if (symbol->scope->getParent() == nullptr) {
        // Create or get the Global Variable
        // getOrInsertGlobal returns a Constant*, so we cast to GlobalVariable*
        auto* globalVariable = llvm::cast<llvm::GlobalVariable>(
            _module->getOrInsertGlobal(node.name, type)
        );

        if (node.value) {
            // If there's an assignment like 'var x = 10', we try to get a constant
            // Note: This only works if node.value is a literal (constant)
            if (auto* constantInit = llvm::dyn_cast<llvm::Constant>(node.value->acceptIR(*this))) {
                globalVariable->setInitializer(constantInit);
            } else {
                // If it's complex (like 'var x = someFunc()'), we set to null
                // and would need an 'init' function (advanced). For now, null it.
                globalVariable->setInitializer(llvm::Constant::getNullValue(type));
            }
        } else {
            globalVariable->setInitializer(llvm::Constant::getNullValue(type));
        }

        _namedValues[symbol] = globalVariable;
        return globalVariable;
    } else {
        // LOCAL VARIABLE PATH
        const auto alloca    = initLocalVariable(type, node.name, node.value->acceptIR(*this));
        _namedValues[symbol] = alloca;

        return alloca;
    }
}

llvm::Value* IRGeneratorLLVM::visit(const VariableAssignment& node) {
    const auto variableAddress = _namedValues.at(node.symbol);
    _builder->CreateStore(node.assignedValue->acceptIR(*this), variableAddress);

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const ExpressionStatement& node) {
    return node.expression->acceptIR(*this);
}

llvm::Value* IRGeneratorLLVM::visit(const ReturnStatement& node) {
    llvm::Value* value = node.value->acceptIR(*this);
    return _builder->CreateRet(value);
}

llvm::Value* IRGeneratorLLVM::visit(const BinaryExpression& node) {
    llvm::Value* lhs = node.left->acceptIR(*this);
    llvm::Value* rhs = node.right->acceptIR(*this);

    promoteToMatch(lhs, rhs);

    const bool isFloat = lhs->getType()->isFloatingPointTy();

    switch (node.op) {
        case Operator::Plus:
            return isFloat
                       ? _builder->CreateFAdd(lhs, rhs, "addtmp")
                       : _builder->CreateAdd(lhs, rhs, "addtmp");
        case Operator::Minus:
            return isFloat
                       ? _builder->CreateFSub(lhs, rhs, "subtmp")
                       : _builder->CreateSub(lhs, rhs, "subtmp");
        case Operator::Star:
            return isFloat
                       ? _builder->CreateFMul(lhs, rhs, "multmp")
                       : _builder->CreateMul(lhs, rhs, "multmp");
        case Operator::Slash:
            return isFloat
                       ? _builder->CreateFDiv(lhs, rhs, "divtmp")
                       : _builder->CreateSDiv(lhs, rhs, "divtmp");

        case Operator::Equal:
            return isFloat
                       ? _builder->CreateFCmpOEQ(lhs, rhs, "eqtmp")
                       : _builder->CreateICmpEQ(lhs, rhs, "eqtmp");
        case Operator::NotEqual:
            return isFloat
                       ? _builder->CreateFCmpONE(lhs, rhs, "neqtmp")
                       : _builder->CreateICmpNE(lhs, rhs, "neqtmp");
        case Operator::GreaterThan:
            // Integers use ICmp, Floats use FCmp
            // UGT = Unsigned Greater Than, SGT = Signed Greater Than, OGT = Ordered Greater Than
            return isFloat
                       ? _builder->CreateFCmpOGT(lhs, rhs, "cmptmp")
                       : _builder->CreateICmpSGT(lhs, rhs, "cmptmp");
        case Operator::GreaterEqual:
            return isFloat
                       ? _builder->CreateFCmpOGE(lhs, rhs, "cmptmp")
                       : _builder->CreateICmpSGE(lhs, rhs, "cmptmp");
        case Operator::LessThan:
            return isFloat
                       ? _builder->CreateFCmpOLT(lhs, rhs, "cmptmp")
                       : _builder->CreateICmpSLT(lhs, rhs, "cmptmp");
        case Operator::LessEqual:
            return isFloat
                       ? _builder->CreateFCmpOLE(lhs, rhs, "cmptmp")
                       : _builder->CreateICmpSLE(lhs, rhs, "cmptmp");

        case Operator::LogicalAnd:
            return _builder->CreateLogicalAnd(lhs, rhs, "logandtmp");
        case Operator::LogicalOr:
            return _builder->CreateLogicalOr(lhs, rhs, "logortmp");
        case Operator::Exclamation:
            return _builder->CreateNot(lhs, "nottmp");
    }

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const UnaryExpression& node) {
    const auto operand = node.operand->acceptIR(*this);

    switch (node.op) {
        case Operator::Minus:
            return _builder->CreateNeg(operand, "negtmp");
        case Operator::Exclamation:
            return _builder->CreateNot(operand, "lognottmp");
    }
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const IncDecExpression& node) {
    llvm::Value* addr = _namedValues[node.symbol];
    if (!addr) {
        // Handle error: Variable not defined
        return nullptr;
    }

    llvm::Value* oldValue = _builder->CreateLoad(
        compilerTypeToLlvmType(node.type),
        addr,
        node.variableName
    );

    llvm::Value* newValue;
    if (node.op == Operator::PlusPlus) {
        newValue = _builder->CreateAdd(oldValue, _builder->getInt32(1), "inc");
    } else {
        newValue = _builder->CreateSub(oldValue, _builder->getInt32(1), "dec");
    }

    // Store the updated value back to memory
    _builder->CreateStore(newValue, addr);

    // Prefix: return the value AFTER the change (++x)
    // Postfix: return the value BEFORE the change (x++)
    if (node.fix == IncDecExpression::Prefix) {
        return newValue;
    } else {
        return oldValue;
    }
}

llvm::Value* IRGeneratorLLVM::visit(const VariableExpression& node) const {
    llvm::Value* variableAddress = _namedValues.at(node.symbol);
    llvm::Type*  varType         = compilerTypeToLlvmType(node.symbol->type);

    return _builder->CreateLoad(varType, variableAddress, node.name + '_');
}

llvm::Value* IRGeneratorLLVM::visit(const IntegerLiteralNode& node) const {
    return llvm::ConstantInt::get(*_context, llvm::APInt(32, node.value, true));
}

llvm::Value* IRGeneratorLLVM::visit(const StringLiteralNode& node) const {
    return llvm::ConstantDataArray::getString(*_context, node.value, true);
}

llvm::Value* IRGeneratorLLVM::visit(const BooleanLiteralNode& node) const {
    return llvm::ConstantInt::get(*_context, llvm::APInt(1, node.value));
}

llvm::Value* IRGeneratorLLVM::visit(const FloatLiteralNode& node) const {
    return llvm::ConstantFP::get(*_context, llvm::APFloat(node.value));
}

llvm::Type* IRGeneratorLLVM::compilerTypeToLlvmType(const Type& type) const {
    switch (type) {
        case Type::Int:
            return _builder->getInt32Ty();
        case Type::Float:
            return _builder->getFloatTy();
        case Type::Boolean:
            return _builder->getInt1Ty();
        case Type::Void:
            return _builder->getVoidTy();
        case Type::Ptr:
            throw std::runtime_error("Not Implemented: pointers");
        case Type::String:
            throw std::runtime_error("Not Implemented: strings");
        case Type::Color:
            throw std::runtime_error("Not Implemented: colors");
        default:
            throw std::runtime_error("Internal error: illegal type: " + typeToString(type));
    }
}

Type IRGeneratorLLVM::llvmTypeToCompilerType(const llvm::Type& type) {
    if (type.isIntegerTy(32)) {
        return Type::Int;
    }

    if (type.isIntegerTy(1)) {
        return Type::Boolean;
    }

    if (type.isFloatTy()) {
        return Type::Float;
    }

    if (type.isVoidTy()) {
        return Type::Void;
    }

    if (type.isPointerTy()) {
        return Type::Ptr;
    }

    throw std::runtime_error("Internal error: unsupported LLVM type: ");
}


void IRGeneratorLLVM::promoteToMatch(llvm::Value*& lhs, llvm::Value*& rhs) const {
    const Type leftType  = llvmTypeToCompilerType(*lhs->getType());
    const Type rightType = llvmTypeToCompilerType(*rhs->getType());

    if (leftType == rightType) return;

    // only allowed promotion: int -> float
    if (leftType == Type::Int && rightType == Type::Float) {
        lhs = _builder->CreateSIToFP(lhs, _builder->getFloatTy());
        return;
    }

    if (leftType == Type::Float && rightType == Type::Int) {
        rhs = _builder->CreateSIToFP(rhs, _builder->getFloatTy());
        return;
    }

    throw std::runtime_error("Internal error: Invalid operand types for binary operation");
}

llvm::Value* IRGeneratorLLVM::initLocalVariable(llvm::Type*  type, const std::string& name,
                                                llvm::Value* value) const {
    const auto alloca = _builder->CreateAlloca(type, nullptr, name);

    if (value) {
        _builder->CreateStore(value, alloca);
    }
    return alloca;
}


void IRGeneratorLLVM::print() const {
    _module->print(llvm::outs(), nullptr);
    if (llvm::verifyModule(*_module, &llvm::errs())) {
        std::cerr << "LLVM Module is broken! See errors above." << std::endl;
    }
}
