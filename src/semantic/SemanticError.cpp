#include "SemanticError.h"
#include <utility>
#include <variant>
#include <format>
#include "parse/AST/AstNode.h"

// Helper for visiting the variant
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

SemanticError::SemanticError(SemanticErrorType type, const AstNode& node, ErrorContext context)
    : CompilerError(node.metadata), _type(type), _node(&node), _context(std::move(context)) {
    // First, handle the data inside the variant
    std::string detail = std::visit(overloaded{
                                        [](std::monostate) -> std::string { return ""; },
                                        [](const std::string& s) { return s; },
                                        [](const TypeMismatchData& d) {
                                            return std::format("expected '{}', but found '{}'",
                                                               typeToString(d.expected), typeToString(d.actual));
                                        },
                                        [](const ParamMismatchData& d) {
                                            return std::format("function '{}' expects {} arguments, but got {}",
                                                               d.name, d.expectedCount, d.actualCount);
                                        },
                                        [](const OperatorData& d) {
                                            return std::format("operator '{}' not defined for types '{}' and '{}'",
                                                               operatorToString(d.op), typeToString(d.left),
                                                               typeToString(d.right));
                                        }
                                    }, _context);

    // Map the Enum to a prefix and combine with the detail
    std::string prefix;
    switch (type) {
        case SemanticErrorType::UndefinedIdentifier:
            prefix = "Undefined identifier";
            break;
        case SemanticErrorType::UndefinedFunction:
            prefix = "Call to undefined function";
            break;
        case SemanticErrorType::TypeMismatch:
            prefix = "Type mismatch";
            break;
        case SemanticErrorType::IncompatibleAssignment:
            prefix = "Incompatible types in assignment";
            break;
        case SemanticErrorType::MissingReturn:
            prefix = "Control reaches end of non-void function";
            break;
        case SemanticErrorType::NonBooleanCondition:
            prefix = "Condition expression must be of boolean type";
            break;
        case SemanticErrorType::ArgumentCountMismatch:
            prefix = "Invalid number of arguments";
            break;
        case SemanticErrorType::DuplicateDeclaration:
            prefix = "Redeclaration of symbol";
            break;
        case SemanticErrorType::CannotInferType:
            prefix = "Cannot infer type of variable without initializer";
            break;
        case SemanticErrorType::ReadOnlyAssignment:
            prefix = "Cannot assign to a read-only (const) variable";
            break;
        case SemanticErrorType::UndefinedParameter:
            prefix = "Function has no parameter with the given name";
            break;
        case SemanticErrorType::ArgumentTypeMismatch:
            prefix = "Argument type mismatch";
            break;
        case SemanticErrorType::DuplicateParameterName:
            prefix = "Duplicate parameter name in function definition";
            break;
        case SemanticErrorType::IncompatibleReturnType:
            prefix = "Value of return expression does not match function return type";
            break;
        case SemanticErrorType::NonNumericRange:
            prefix = "Range bounds must be numeric";
            break;
        case SemanticErrorType::NonNumericStep:
            prefix = "Loop step must be a numeric value";
            break;
        case SemanticErrorType::OperatorNotDefined:
            prefix = "Binary operator not defined for these operands";
            break;
        case SemanticErrorType::UnaryOperatorMismatch:
            prefix = "Unary operator not defined for this operand type";
            break;
        default:
            prefix = "Semantic error";
            break;
    }

    // Combine into the final message
    _message = detail.empty() ? prefix : std::format("{}: {}", prefix, detail);
}
