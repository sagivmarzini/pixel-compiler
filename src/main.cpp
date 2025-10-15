#include "lex/Lexer.h"
#include "Timer.h"

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
    
    //extract all text to source
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    auto source = buffer.str();
    

    std::cout << "Starting Compilation\n\n";
    Timer timer = Timer();



    Lexer lexer = Lexer(source);

    timer.start();
    auto tokens = lexer.lex();
    timer.stop("Lexing");
    std::cout << lexer.printTokens(tokens);
    
    return 0;
}