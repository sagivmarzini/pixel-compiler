#include <fstream>

#include "Compiler.h"

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

    Compiler compiler = Compiler(buffer.str());

    return 0;
}