#include "Compiler.h"

#include <fstream>
#include <iostream>


int main(int argc, char **argv) {
    if (argc != 2)
        throw std::runtime_error("Please add input file as an argument!");

    // read input file
    std::fstream file(argv[1]);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file " + std::string(argv[1]) + "!\n");

    // extract all text to source
    // std::ostringstream buffer;
    // buffer << file.rdbuf();
    // file.close();
    std::string sourceCode;
    std::cout << "Enter your code: ";
    std::cin >> sourceCode;

    Compiler compiler(sourceCode);

    return 0;
}
