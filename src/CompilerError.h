#ifndef COMPILER_PROJECT_COMPILERERROR_H
#define COMPILER_PROJECT_COMPILERERROR_H
#include <string>

#include "lex/Token.h"

class CompilerError {
public:
    explicit CompilerError(const TokenMetadata& metadata);

    [[nodiscard]] std::string getMessage() const;

    [[nodiscard]] TokenMetadata getMetadata() const;

protected:
    std::string   _message;
    TokenMetadata _metadata;
};


#endif //COMPILER_PROJECT_COMPILERERROR_H
