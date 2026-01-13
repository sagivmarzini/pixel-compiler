//
// Created by Sagiv Marzini on 13/01/2026.
//

#ifndef COMPILER_PROJECT_SEMANTICERROR_H
#define COMPILER_PROJECT_SEMANTICERROR_H
#include <utility>

#include "CompilerError.h"

class AstNode;

// Define the specific data packets for different errors
struct TypeMismatchData {
    Type expected;
    Type actual;
};

struct ParamMismatchData {
    std::string name;
    int         expectedCount;
    int         actualCount;
};

struct OperatorData {
    Operator op;
    Type     left;
    Type     right;
};

using ErrorContext = std::variant<
    std::monostate, // No extra info
    TypeMismatchData,
    ParamMismatchData,
    OperatorData,
    std::string // Generic fallback
>;

enum class SemanticErrorType {
    // Variable & Scoping
    UndefinedIdentifier,
    DuplicateDeclaration,
    CannotInferType,
    ReadOnlyAssignment, // For const

    // Function/Call Issues
    UndefinedFunction,
    UndefinedParameter,
    ArgumentCountMismatch,
    ArgumentTypeMismatch,
    DuplicateParameterName,

    // Type Logic
    TypeMismatch, // General purpose
    IncompatibleAssignment,
    IncompatibleReturnType,

    // Control Flow
    NonBooleanCondition, // If/While
    NonNumericRange,     // For loops
    NonNumericStep,
    MissingReturn,

    // Operators
    OperatorNotDefined,    // e.g. "String - String"
    UnaryOperatorMismatch, // e.g. "!5"

    // Declaration pass
    ParameterRedeclaration,
};

class SemanticError : public CompilerError {
public:
    SemanticError(SemanticErrorType type, const AstNode& node, ErrorContext context = std::monostate{});

private:
    SemanticErrorType _type;
    const AstNode*    _node;
    ErrorContext      _context;
};


#endif //COMPILER_PROJECT_SEMANTICERROR_H
