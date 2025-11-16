#include "Lexer.h"

Lexer::Lexer(std::string src) : source(src) {}

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
                if (type == TokenType::Comment) {}
                    // ignore comments

                // check if string was a keyword
                else if (type == TokenType::Identifier)
                    tokens.push_back( {getKeyword(match.str()), match.str()});

                else if (type == TokenType::Literal_Number)
                    tokens.push_back({TokenType::Literal_Number, match.str()});

                else if (type == TokenType::Literal_String)
                    tokens.push_back({TokenType::Literal_String, match.str()});

                else
                    tokens.push_back({ type});

                it += match.length();
                wasMatched = true;
                break;
            }
        }

        if (!wasMatched)
            throw std::runtime_error(std::string("Lexer Error: Unknown character starting from '") + *it + "'");
    }

    tokens.push_back({TokenType::EndOfFile, "EOF"});
    return tokens;
}

TokenType Lexer::getKeyword(const std::string& value)
{
    if (KEYWORDS.contains(value))
        return KEYWORDS.at(value);

    return TokenType::Identifier;
}


const std::string Lexer::printTokens(const std::vector<Token> &tokens)
{
    std::stringstream out;

    out << "result:\n[";
    for (int i = 0; i < tokens.size(); i++)
    {
        tokens[i].print(out);

        if (i+1 != tokens.size())
            out << ", ";
    }
    out << "]\n\n";
    return out.str();
}
