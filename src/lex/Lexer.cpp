#include "Lexer.h"

#include <vector>

Lexer::Lexer(std::string src)
{
    source = std::string(src);   
}

Lexer::~Lexer() { }

std::vector<Token> Lexer::lex()
{
    std::vector<Token> tokens;

    std::smatch match;
    auto it = source.cbegin();

    while (it != source.cend())
    {
        if (std::isspace(*it)) 
        {
            it++;
            continue;
        }

        bool wasMatched = false;

        for (auto &[pattern, type] : rules)
        {
            if (std::regex_search(it, source.cend(), match, pattern, std::regex_constants::match_continuous))
            {
                std::string value = match.str();

                // check if string was a keyword
                if (type == TokenType::Identifier)
                {
                    tokens.push_back( {getKeyword(value), value} );
                }
                else
                {
                    tokens.push_back({ type, value});
                }
                
                it += match.length();
                wasMatched = true;
                break;
            }
            
        }

        if (!wasMatched)
        {
            throw std::string("Unknown character at '") + *it + "'";
        }
    }


    return tokens;
}

TokenType Lexer::getKeyword(const std::string& value)
{
    auto iterator = KEYWORDS.find(value);
    if (iterator != KEYWORDS.end())
    {
        return iterator->second;
    }
    return TokenType::Identifier;
}


#include "../magic_enum/magic_enum.hpp"

const std::string Lexer::printTokens(const std::vector<Token> &tokens)
{
    std::stringstream out;
    out << "[";
    for (int i = 0; i < tokens.size(); i++)
    {
        out << magic_enum::enum_name(tokens[i].type) << "(" << tokens[i].value << "), ";
    }
    out << "]\n";
    return out.str();
}
