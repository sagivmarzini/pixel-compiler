#include "lex/Lexer.h"

#include <iostream>
#include <fstream>


//compile & run with:
//g++ src/main.cpp src/*/* -o main && ./main source.sgivush

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "please add input file as argument\n";
        return -1;
    }

    //read input file
    std::fstream file(argv[1]); 
    if (!file.is_open())
    {
        std::cerr << "failed to open file " << argv[1] << "!\n";
        return -2;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    Lexer lexer = Lexer(buffer.str());

    auto tokens = lexer.lex();
    std::cout << lexer.printTokens(tokens);
    
    return 0;
}