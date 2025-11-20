#include "Parser.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

