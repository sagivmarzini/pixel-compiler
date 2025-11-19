#include "Lexer.h"

#include <iostream>

const std::vector<std::pair<std::regex, TokenType>> rules = {
    // important! must put the longer stuff first so
    // == will parse as '==' and not '=' + '=' (equals instead of 2 asignments)

    // []- group stuff together: [a-zA-Z] is lowercase and upercase letters
    // + - one or more charecters: [a-z]+ will catch helo but not empty string
    // \\- the literal charecter: [a-z \\[ \\] ] will include a-z and '[', ']'
    // ^ - starts with: [1-3] will catch 321 from 54321, while [^1-3] will not

    {std::regex("(//[^\\n]*|/\\*([^*]|\\*+[^*/])*\\*+/)"), TokenType::Comment},


    {std::regex("^[a-zA-Z_][a-zA-Z0-9_]*"), TokenType::Identifier},

    {std::regex("[0-9]*?\\.[0-9]+"), TokenType::LiteralNumber},
    {std::regex("^[0-9]+"), TokenType::LiteralNumber},
    {std::regex(R"("[^"]*")"), TokenType::LiteralString},

    // --- symbols ---
    { std::regex("^;"), TokenType::SymbolSemicolon },
    {std::regex("^:"), TokenType::SymbolColon},
    {std::regex("^,"), TokenType::SymbolComma},
    { std::regex("^\\{"), TokenType::SymbolLBrace },
    { std::regex("^\\}"), TokenType::SymbolRBrace },
    { std::regex("^\\("), TokenType::SymbolLParen },
    { std::regex("^\\)"), TokenType::SymbolRParen },
    {std::regex("^->"), TokenType::SymbolArrow},
    {std::regex("\\.\\."), TokenType::SymbolDoublePeriod},

    // opps (longer first)
    { std::regex("^=="), TokenType::OperatorEqual },
    { std::regex("^!="), TokenType::OperatorNotEqual },
    { std::regex("^<="), TokenType::OperatorLessEqual },
    { std::regex("^>="), TokenType::OperatorGreaterEqual },
    { std::regex("^&&"), TokenType::OperatorAnd },
    { std::regex("^\\|\\|"), TokenType::OperatorOr },

    { std::regex("^="),  TokenType::OperatorAssignment },
    { std::regex("^!"),  TokenType::OperatorExclamation },
    { std::regex("^<"),  TokenType::OperatorLess },
    { std::regex("^>"),  TokenType::OperatorGreater },

    { std::regex("^\\+"), TokenType::OperatorPlus },
    { std::regex("^-"),  TokenType::OperatorMinus },
    { std::regex("^\\*"), TokenType::OperatorStar },
    { std::regex("^/"),  TokenType::OperatorSlash },
};

const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"true", TokenType::LiteralTrue},
    {"false", TokenType::LiteralFalse},

    {"int", TokenType::TypeInt},
    {"float", TokenType::TypeFloat},
    {"bool", TokenType::TypeBool},
    {"ptr", TokenType::TypePtr},
    {"string", TokenType::TypeString},
    {"color", TokenType::TypeColor},
    {"void", TokenType::TypeVoid},


    {"func", TokenType::KeywordFunc},
    {"var", TokenType::KeywordVar},
    {"return", TokenType::KeywordReturn},

    {"if", TokenType::KeywordIf},
    {"else", TokenType::KeywordElse},
    {"while", TokenType::KeywordWhile},
    {"for", TokenType::KeywordFor},
};


Lexer::Lexer(std::string& sourceCode) : _sourceCode(sourceCode) {}

std::vector<TokenType> Lexer::lex() const
{
    std::vector<TokenType> tokens;

    std::smatch match;
    auto currentChar = _sourceCode.cbegin();

    while (currentChar != _sourceCode.cend())
    {
        if (std::isspace(*currentChar))
        {
            currentChar++;
            continue;
        }

        bool wasMatched = false;

        for (auto &[pattern, type] : rules)
        {
            if (std::regex_search(currentChar, _sourceCode.cend(), match, pattern, std::regex_constants::match_continuous))
            {
                if (type == TokenType::Comment) {}
                    // ignore comments

                // check if string was a keyword
                else if (type == TokenType::Identifier)
                    tokens.push_back({getKeyword(match.str()), match.str()});

                else if (type == TokenType::LiteralNumber)
                    tokens.push_back({TokenType::LiteralNumber, match.str()});

                else if (type == TokenType::LiteralString)
                    tokens.push_back({TokenType::LiteralString, match.str()});

                else
                    tokens.push_back({type});

                currentChar += match.length();
                wasMatched = true;
                break;
            }
        }

        if (!wasMatched)
            throw std::runtime_error(std::string("Lexer Error: Unknown character starting from '") + *currentChar + "'");
    }

    tokens.push_back({TokenType::EndOfFile, "EOF"});
    return tokens;
}

TokenType Lexer::getKeyword(const std::string& value) const
{
    if (KEYWORDS.contains(value))
        return KEYWORDS.at(value);

    return TokenType::Identifier;
}

void Lexer::printTokens(const std::vector<TokenType> &tokens)
{
    std::cout << "result:\n[";
    for (int i = 0; i < tokens.size(); i++)
    {
        std::cout << tokens[i];

        if (i+1 != tokens.size())
            std::cout << ", ";
    }
    std::cout << "]\n\n";
}
