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


const std::string Lexer::printTokens(const std::vector<Token> &tokens)
{
    std::stringstream out;

    const std::unordered_map<TokenType, std::string> typeToName = {
        {TokenType::Identifier, "Identifier"},
        {TokenType::Integer, "Integer"},
        {TokenType::EndOfFile, "EndOfFile"},
        {TokenType::Keyword_Function, "Keyword_Function"},
        {TokenType::Keyword_Return, "Keyword_Return"},
        {TokenType::Keyword_Var, "Keyword_Var"},
        {TokenType::Keyword_If, "Keyword_If"},
        {TokenType::Keyword_Else, "Keyword_Else"},
        {TokenType::Keyword_While, "Keyword_While"},
        {TokenType::Keyword_For, "Keyword_For"},
        {TokenType::Symbol_Semicolon, "Symbol_Semicolon"},
        {TokenType::Symbol_LBrace, "Symbol_LBrace"},
        {TokenType::Symbol_RBrace, "Symbol_RBrace"},
        {TokenType::Symbol_LParen, "Symbol_LParen"},
        {TokenType::Symbol_RParen, "Symbol_RParen"},
        {TokenType::Operator_Assignment, "Operator_Assignment"},
        {TokenType::Operator_Plus, "Operator_Plus"},
        {TokenType::Operator_Minus, "Operator_Minus"},
        {TokenType::Operator_Star, "Operator_Star"},
        {TokenType::Operator_Slash, "Operator_Slash"},
        {TokenType::Operator_And, "Operator_And"},
        {TokenType::Operator_Or, "Operator_Or"},
        {TokenType::Operator_Equal, "Operator_Equal"},
        {TokenType::Operator_NotEqual, "Operator_NotEqual"},
        {TokenType::Operator_Exclamation, "Operator_Exclamation"},
        {TokenType::Operator_Less, "Operator_Less"},
        {TokenType::Operator_LessEqual, "Operator_LessEqual"},
        {TokenType::Operator_Greater, "Operator_Greater"},
        {TokenType::Operator_GreaterEqual, "Operator_GreaterEqual"},
    };

    out << "result:\n[";
    for (int i = 0; i < tokens.size(); i++)
    {
        out << typeToName.at(tokens[i].type);
        if (tokens[i].type == TokenType::Identifier || tokens[i].type == TokenType::Integer)
        {
            out << '(' << tokens[i].value << ')';
        }
        out << ", ";
    }
    out << "]\n";
    return out.str();
}
