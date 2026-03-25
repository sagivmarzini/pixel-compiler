#ifndef COMPILER_PROJECT_SEMANTICERROR_H
#define COMPILER_PROJECT_SEMANTICERROR_H
#include <utility>

#include "CompilerError.h"
#include "parse/AST/Expression.h"
#include "parse/AST/Statement.h"

namespace AST {
    class Node;
}

// Define the specific data packets for different errors
struct TypeMismatchData {
    ScalarKind expected;
    ScalarKind actual;
};

struct ParamMismatchData {
    std::string name;
    int expectedCount;
    int actualCount;
};

struct OperatorData {
    Operator op;
    ScalarKind left;
    ScalarKind right;
};

struct UnaryOperatorData {
    Operator op;
    ScalarKind operand;
};

struct ArgumentPositionData {
    std::string parameters;
    std::string arguments;

    ArgumentPositionData(std::vector<AST::FunctionCall::FunctionArgument>& args,
                         const std::vector<AST::FunctionDeclaration::FunctionParameter>& params) {
        for (int i = 0; i < args.size(); i++) {
            auto param = params[i].isImplicit ? params[i].name : "_";
            parameters.append(param + ", ");
            arguments.append(args[i].name.value_or("_") + ", ");
        }
    }
};

using ErrorContext = std::variant<
    std::monostate, // No extra info
    TypeMismatchData,
    ParamMismatchData,
    OperatorData,
    UnaryOperatorData,
    ArgumentPositionData,
    std::string // Generic fallback
>;

enum class SemanticErrorType {
    // Variable & Scoping
    UndefinedIdentifier,
    DuplicateDeclaration,
    CannotInferType,
    ReadOnlyAssignment, // For const
    NonIntIndex,

    // Function/Call Issues
    UndefinedFunction,
    UndefinedArgument,
    ArgumentCountMismatch,
    ArgumentTypeMismatch,
    DuplicateParameterName,
    DuplicateArgumentName,
    MissingArgumentLabel,
    InvalidArgumentPosition,
    OutOfBounds,


    // Type Logic
    TypeMismatch, // General purpose
    IncompatibleAssignment,
    IncompatibleReturnType,
    MultiTypeArray,

    // Control Flow
    NonBooleanCondition, // If/While
    NonNumericRange, // For loops
    NonNumericStep,
    NonIntForLoop,
    MissingReturn,
    UnreachableCode,

    // Operators
    OperatorNotDefined, // e.g. "String - String"
    UnaryOperatorMismatch, // e.g. "!5"

    // Declaration pass
    ParameterRedeclaration,
    MissingMainFunction,
};

class SemanticError : public CompilerError {
public:
    SemanticError(SemanticErrorType type, const AST::AstNode& node, ErrorContext context = std::monostate{});

private:
    SemanticErrorType _type;
    const AST::AstNode* _node;
    ErrorContext _context;
};


#endif //COMPILER_PROJECT_SEMANTICERROR_H
