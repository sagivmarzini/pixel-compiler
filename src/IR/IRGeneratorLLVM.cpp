//
// Created by Sagiv Marzini on 14/01/2026.
//

#include "IRGeneratorLLVM.h"

#include "parse/AST/Expression.h"
#include "parse/AST/Statement.h"
#include "semantic/Symbol.h"
#include "semantic/SymbolTable.h"
#include "semantic/FunctionSignature.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <vector>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>


IRGeneratorLLVM::IRGeneratorLLVM() {
    _context = std::make_unique<llvm::LLVMContext>();
    _module  = std::make_unique<llvm::Module>("pixel_compiler", *_context);
    _builder = std::make_unique<llvm::IRBuilder<> >(*_context);
}

IRGeneratorLLVM::IRGeneratorLLVM(
    std::unordered_map<std::string, FunctionSignature> builtInFunctions)
    : IRGeneratorLLVM() {
    _builtinFunctions = std::move(builtInFunctions);
}

llvm::Value* IRGeneratorLLVM::visit(const AST::Program& program) {
    for (const auto& stmt: program.statements) {
        stmt->acceptIR(*this);
    }
    // Return the module at the end for printing/saving
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const AST::FunctionDeclaration& node) {
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
        if (node.returnType == Type::Void) {
            _builder->CreateRetVoid();
        } else {
            // Return a default "zero" value for the specific type
            _builder->CreateRet(llvm::Constant::getNullValue(compilerTypeToLlvmType(node.returnType)));
        }
    }

    return function;
}

llvm::Value* IRGeneratorLLVM::visit(const AST::FunctionCall& node) {
    const auto calledFunction = getOrDeclareBuiltin(node.functionName);
    if (!calledFunction) {
        throw std::runtime_error("| LLVM | Function not found: " + node.functionName);
    }

    std::vector<llvm::Value*> args;
    bool                      isVariadic = calledFunction->isVarArg();
    int                       index      = 0;
    for (const auto& arg: node.arguments) {
        auto argValue = arg.value->acceptIR(*this);

        // Promote argument to float if necessary
        if (index < calledFunction->getFunctionType()->getNumParams()) {
            // This is a fixed argument, we can safely cast to the expected type
            argValue = castToExpectedType(argValue, calledFunction->getArg(index)->getType());
        } else if (isVariadic) {
            // Variadic arguments (the values after the format string)
            // If the value is a 32-bit Float, promote it to a 64-bit Double
            if (argValue->getType()->isFloatTy()) {
                argValue = _builder->CreateFPExt(argValue, _builder->getDoubleTy(), "float_to_double");
            }
        }

        args.push_back(argValue);
        if (!args.back())
            return nullptr;

        index++;
    }

    return _builder->CreateCall(calledFunction, args);
}

llvm::Value* IRGeneratorLLVM::visit(const AST::IfStatement& node) {
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

llvm::Value* IRGeneratorLLVM::visit(const AST::WhileLoop& node) {
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

llvm::Value* IRGeneratorLLVM::visit(const AST::ForLoop& node) {
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

llvm::Value* IRGeneratorLLVM::visit(const AST::RangeExpression& node) {
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const AST::Block& node) {
    for (const auto& stmt: node.statements) {
        stmt->acceptIR(*this);
    }
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const AST::VariableDeclaration& node) {
    const auto symbol = node.symbol;
    const auto type   = compilerTypeToLlvmType(symbol->type);

    if (symbol->scope->getParent()->getParent() == nullptr) {
        // Check if the value is a string literal
        if (auto* strLit = dynamic_cast<AST::StringLiteralNode*>(node.value.get())) {
            // 1. Create the string data
            auto* stringData = llvm::ConstantDataArray::getString(*_context, strLit->value, true);

            // 2. Create the Global Variable
            auto* gVar = new llvm::GlobalVariable(
                *_module, stringData->getType(), // The type is [N x i8]
                true,                            // isConstant
                llvm::GlobalValue::InternalLinkage,
                stringData, // Initializer
                node.name
            );

            _namedValues[node.symbol] = gVar;
            return gVar;
        }

        // GLOBAL VARIABLE PATH
        // Check if already exists, otherwise create
        llvm::GlobalVariable* globalVariable = _module->getGlobalVariable(node.name);

        if (!globalVariable) {
            globalVariable = new llvm::GlobalVariable(
                *_module, type, false,
                llvm::GlobalValue::ExternalLinkage,
                nullptr, node.name
            );
        }

        if (node.value) {
            auto* initValue = node.value->acceptIR(*this);
            if (auto* constantInit = llvm::dyn_cast<llvm::Constant>(initValue)) {
                // If types don't match exactly (e.g. i32 init for float global), cast it
                if (constantInit->getType() != type) {
                    constantInit = llvm::ConstantExpr::getBitCast(constantInit, type);
                }
                globalVariable->setInitializer(constantInit);
            } else {
                throw std::runtime_error("Global variables must be initialized with a constant.");
            }
        } else {
            globalVariable->setInitializer(llvm::Constant::getNullValue(type));
        }

        _namedValues[symbol] = globalVariable;
        return globalVariable;
    } else {
        // LOCAL VARIABLE PATH
        llvm::Value* val = node.value ? node.value->acceptIR(*this) : nullptr;

        if (val) {
            val = castToExpectedType(val, type); // Ensure 0 becomes 0.0 before storage
        }

        const auto alloca    = initLocalVariable(type, node.name, val);
        _namedValues[symbol] = alloca;
        return alloca;
    }
}

llvm::Value* IRGeneratorLLVM::visit(const AST::VariableAssignment& node) {
    const auto variableAddress = _namedValues.at(node.symbol);
    _builder->CreateStore(node.assignedValue->acceptIR(*this), variableAddress);

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const AST::ExpressionStatement& node) {
    return node.expression->acceptIR(*this);
}

llvm::Value* IRGeneratorLLVM::visit(const AST::ReturnStatement& node) {
    llvm::Value* value = node.value->acceptIR(*this);

    llvm::Type* expectedType = _builder->GetInsertBlock()->getParent()->getReturnType();
    value                    = castToExpectedType(value, expectedType);

    return _builder->CreateRet(value);
}

llvm::Value* IRGeneratorLLVM::visit(const AST::BinaryExpression& node) {
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

llvm::Value* IRGeneratorLLVM::visit(const AST::UnaryExpression& node) {
    const auto operand = node.operand->acceptIR(*this);

    // 1. CONSTANT PATH (For Global Initializers)
    if (auto* c = llvm::dyn_cast<llvm::ConstantFP>(operand)) {
        if (node.op == Operator::Minus) {
            llvm::APFloat val = c->getValueAPF();
            val.changeSign();
            return llvm::ConstantFP::get(c->getContext(), val);
        }
    }

    // For Integers
    if (auto* c = llvm::dyn_cast<llvm::ConstantInt>(operand)) {
        if (node.op == Operator::Minus) {
            return llvm::ConstantInt::get(c->getContext(), -c->getValue());
        }
    }

    // 2. INSTRUCTION PATH (For Local Variables / Functions)
    switch (node.op) {
        case Operator::Minus:
            if (operand->getType()->isFloatingPointTy()) {
                return _builder->CreateFNeg(operand, "fnegtmp");
            }
            return _builder->CreateNeg(operand, "negtmp");

        case Operator::Exclamation:
            // CreateNot works for i1 (booleans)
            return _builder->CreateNot(operand, "lognottmp");
    }
    return nullptr;
}

llvm::Value* IRGeneratorLLVM::visit(const AST::IncDecExpression& node) {
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
    if (node.fix == AST::IncDecExpression::Prefix) {
        return newValue;
    } else {
        return oldValue;
    }
}

llvm::Value* IRGeneratorLLVM::visit(const AST::VariableExpression& node) const {
    llvm::Value* variableAddress = _namedValues.at(node.symbol);
    llvm::Type*  varType         = compilerTypeToLlvmType(node.symbol->type);

    // Strings are arrays in memory; we want the pointer to the first element
    if (node.symbol->type == Type::String) {
        // If it's a GlobalVariable, it has an 'allocated' or 'value' type (the array)
        if (auto* gVar = llvm::dyn_cast<llvm::GlobalVariable>(variableAddress)) {
            llvm::Type* gType = gVar->getValueType();
            if (gType->isArrayTy()) {
                // Decay the array [N x i8] to a simple ptr
                // In modern LLVM, CreateGEP requires the source element type explicitly
                return _builder->CreateInBoundsGEP(
                    gType,
                    variableAddress,
                    {_builder->getInt64(0), _builder->getInt64(0)},
                    "strdecay"
                );
            }
        }
        // If it's already a ptr (like a function argument), just return it
        return variableAddress;
    }

    // For non-strings, perform a standard load
    return _builder->CreateLoad(varType, variableAddress, node.name + '_');
}

llvm::Value* IRGeneratorLLVM::visit(const AST::IntegerLiteralNode& node) const {
    return llvm::ConstantInt::get(*_context, llvm::APInt(32, node.value, true));
}

llvm::Value* IRGeneratorLLVM::visit(const AST::StringLiteralNode& node) const {
    // If we are inside a function, this is safe
    if (_builder->GetInsertBlock()) {
        return _builder->CreateGlobalString(node.value, "strtmp");
    }

    // If we are NOT in a function (Global Init), return the raw constant array
    return llvm::ConstantDataArray::getString(*_context, node.value, true);
}

llvm::Value* IRGeneratorLLVM::visit(const AST::BooleanLiteralNode& node) const {
    return llvm::ConstantInt::get(*_context, llvm::APInt(1, node.value));
}

llvm::Value* IRGeneratorLLVM::visit(const AST::FloatLiteralNode& node) const {
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
            // throw std::runtime_error("Not Implemented: strings");
            return _builder->getPtrTy();
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

llvm::Value* IRGeneratorLLVM::castToExpectedType(llvm::Value* value, const llvm::Type* expectedType) const {
    if (value->getType() == expectedType)
        return value;

    if (value->getType() == _builder->getInt32Ty() && expectedType == _builder->getFloatTy())
        return _builder->CreateSIToFP(value, _builder->getFloatTy());

    return nullptr;
}

llvm::Value* IRGeneratorLLVM::initLocalVariable(llvm::Type*  type, const std::string& name,
                                                llvm::Value* value) const {
    const auto alloca = _builder->CreateAlloca(type, nullptr, name);

    if (value) {
        _builder->CreateStore(value, alloca);
    }
    return alloca;
}

llvm::Function* IRGeneratorLLVM::getOrDeclareBuiltin(const std::string& name) {
    std::string actualName = name;

    // 1. SILENT MAPPING LOGIC
    // Check if it's a math function that needs the 'f' suffix for returning 32-bit floats
    // TODO: Use a list of functions that require an f suffix instead
    if (name == "sin" || name == "cos" || name == "tan" || name == "sqrt" || name == "pow" || name == "floor") {
        const auto it = _builtinFunctions.find(name);
        if (it != _builtinFunctions.end()) {
            // If the return type is Float (32-bit), use the 'f' version
            if (it->second.returnType == Type::Float) {
                actualName += "f";
            }
        }
    }

    // 2. Look up using the actualName
    if (auto* function = _module->getFunction(actualName)) {
        return function;
    }

    const auto it = _builtinFunctions.find(name); // Still find metadata by original name
    if (it == _builtinFunctions.end()) {
        return nullptr;
    }

    const auto               [parameters, returnType] = it->second;
    std::vector<llvm::Type*> paramTypes;
    for (const auto& parameter: parameters) {
        paramTypes.push_back(compilerTypeToLlvmType(parameter.type));
    }

    llvm::Type* retType    = compilerTypeToLlvmType(returnType);
    bool        isVariadic = (name == "printf");
    auto*       funcType   = llvm::FunctionType::get(retType, paramTypes, isVariadic);

    // 3. Create the function with the 'actualName' (e.g., sinf)
    return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, actualName, _module.get());
}


void IRGeneratorLLVM::print() const {
    _module->print(llvm::outs(), nullptr);
    if (llvm::verifyModule(*_module, &llvm::errs())) {
        std::cerr << "LLVM Module is broken! See errors above." << std::endl;
    }
}

void IRGeneratorLLVM::createExecutable(const std::string& outputPath) const {
    // 1. Initialize LLVM targets for the native architecture (x86_64)
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    // 2. Get the target triple (e.g., "x86_64-pc-linux-gnu")
    auto         targetTripleStr = llvm::sys::getDefaultTargetTriple();
    llvm::Triple targetTriple(targetTripleStr);

    _module->setTargetTriple(targetTriple);

    std::string error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        throw std::runtime_error("| LLVM | Failed to find target: " + error);
    }

    // 3. Configure the Target Machine
    auto                cpu      = "generic";
    auto                features = "";
    llvm::TargetOptions opt;
    auto                targetMachine = target->createTargetMachine(
        targetTriple, cpu, features, opt, llvm::Reloc::PIC_
    );

    _module->setDataLayout(targetMachine->createDataLayout());

    // 4. Emit the Object File (.o)
    std::string          objFilename = outputPath + ".o";
    std::error_code      ec;
    llvm::raw_fd_ostream dest(objFilename, ec, llvm::sys::fs::OF_None);

    if (ec) {
        throw std::runtime_error("| LLVM | Could not open output file: " + ec.message());
    }

    llvm::legacy::PassManager pass;

    auto fileType = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        throw std::runtime_error("| LLVM | TargetMachine can't emit a file of this type");
    }

    // Run the passes to generate machine code
    pass.run(*_module);
    dest.flush();

    // 5. Link the Object File into an Executable using Clang
    // We add -lm to link the math library (for sin, cos, sqrt, pow)
    std::string linkCommand = "clang " + objFilename + " -o " + outputPath + " -lm";

    // If you are ready to link your Bridge and SDL3, uncomment this:
    // linkCommand += " bridge.o -lSDL3";

    std::cout << "Linking..." << std::endl;
    int linkResult = std::system(linkCommand.c_str());

    if (linkResult != 0) {
        throw std::runtime_error("| LLVM | Linking failed! Check if clang is installed.");
    }

    std::cout << "Successfully compiled executable: " << outputPath << std::endl;
}
