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

SemanticError::SemanticError(SemanticErrorType type, const AST::AstNode& node, ErrorContext context)
    : CompilerError(node.metadata), _type(type), _node(&node), _context(std::move(context)) {
    // 1. Format the specific details (The "Why")
    // We format these to be grammatically smooth when appended to the prefix.
    std::string detail = std::visit(overloaded{
                                        [](std::monostate) -> std::string { return ""; },

                                        // Simple string detail (usually the identifier name)
                                        [](const std::string& s) { return std::format("'{}'", s); },

                                        [](const TypeMismatchData& d) {
                                            return std::format("expected type '{}', but found '{}'",
                                                               typeToString(d.expected), typeToString(d.actual));
                                        },

                                        [](const ParamMismatchData& d) {
                                            return std::format(
                                                "function '{}' requires {} arguments, but {} were provided",
                                                d.name, d.expectedCount, d.actualCount);
                                        },

                                        [](const OperatorData& d) {
                                            return std::format(
                                                "cannot apply binary operator '{}' to operands of type '{}' and '{}'",
                                                operatorToString(d.op), typeToString(d.left), typeToString(d.right));
                                        },

                                        [](const UnaryOperatorData& d) {
                                            return std::format(
                                                "cannot apply unary operator '{}' to operand of type '{}'",
                                                operatorToString(d.op), typeToString(d.operand));
                                        },

                                        [](const ArgumentPositionData& d) {
                                            return std::format("signature expects ({}), but call provided ({})",
                                                               d.parameters, d.arguments);
                                        }
                                    }, _context);

    // 2. Define the high-level error category (The "What")
    std::string prefix;
    switch (type) {
        // --- Identifier & Scope Errors ---
        case SemanticErrorType::UndefinedIdentifier:
            prefix = "use of undeclared identifier";
            break;
        case SemanticErrorType::UndefinedFunction:
            prefix = "call to undeclared function";
            break;
        case SemanticErrorType::DuplicateDeclaration:
            prefix = "redefinition of symbol";
            break;
        case SemanticErrorType::DuplicateParameterName:
            prefix = "redefinition of parameter name";
            break;

        // --- Type System Errors ---
        case SemanticErrorType::TypeMismatch:
            // If we have details, the prefix acts as a header.
            prefix = "type mismatch";
            break;
        case SemanticErrorType::IncompatibleAssignment:
            prefix = "cannot assign value of incompatible type";
            break;
        case SemanticErrorType::CannotInferType:
            prefix = "type declarations require an explicit type or an initializer";
            break;
        case SemanticErrorType::IncompatibleReturnType:
            prefix = "return value type does not match function signature";
            break;

        // --- Function Call Errors ---
        case SemanticErrorType::ArgumentCountMismatch:
            prefix = "incorrect number of arguments";
            break;
        case SemanticErrorType::UndefinedArgument:
            prefix = "function does not accept a parameter named"; // detail will be 'name'
            break;
        case SemanticErrorType::ArgumentTypeMismatch:
            prefix = "invalid argument type";
            break;
        case SemanticErrorType::DuplicateArgumentName:
            prefix = "parameter provided multiple times in call";
            break;
        case SemanticErrorType::MissingArgumentLabel:
            prefix = "missing argument label in function call";
            break;
        case SemanticErrorType::InvalidArgumentPosition:
            prefix = "arguments provided in incorrect order";
            break;

        // --- Control Flow & Logic Errors ---
        case SemanticErrorType::MissingReturn:
            prefix = "non-void function does not return a value in all code paths";
            break;
        case SemanticErrorType::NonBooleanCondition:
            prefix = "if/while condition must be a boolean expression";
            break;
        case SemanticErrorType::ReadOnlyAssignment:
            prefix = "cannot assign to immutable variable";
            break;
        case SemanticErrorType::NonNumericRange:
            prefix = "range bounds must be numeric types";
            break;
        case SemanticErrorType::NonNumericStep:
            prefix = "loop step must be a numeric value";
            break;

        // --- Operator Errors ---
        case SemanticErrorType::OperatorNotDefined:
        case SemanticErrorType::UnaryOperatorMismatch:
            // The detail string handles the full explanation here,
            // so we keep the prefix empty or generic if detail is missing.
            prefix = "invalid operation";
            break;

        default:
            prefix = "semantic error";
            break;
        case SemanticErrorType::ParameterRedeclaration:
            prefix = "Function parameter declared more than once";
            break;
        case SemanticErrorType::MissingMainFunction:
            prefix = "No main function found";
            break;
        case SemanticErrorType::UnreachableCode:
            prefix = "Unreachable code";
            break;
    }

    // 3. Construct the Final Message
    // Logic: If the detail is long/complex (contains spaces), we use a colon separator.
    // If the detail is just a variable name (no spaces), we integrate it directly.

    if (detail.empty()) {
        _message = prefix;
    } else if (type == SemanticErrorType::OperatorNotDefined || type == SemanticErrorType::UnaryOperatorMismatch) {
        // For operators, the detail is the whole sentence.
        _message = detail;
    } else {
        _message = std::format("{}: {}", prefix, detail);
    }
}
