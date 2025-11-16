#include "Compiler.h"

Compiler::Compiler(std::string source) : source(source)
{
    std::cout << "Starting compilation\n";

    try
    {
        compile();

    } catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    } catch (...)
    {
        std::cout << "Uncaught exception. Somthing went wrong bro. Good luck, i believe in you.\n";
    }
}

void Compiler::compile()
{
    Lexer lexer = Lexer(source);

    auto tokens = lexer.lex();

    std::cout << lexer.printTokens(tokens);
}
