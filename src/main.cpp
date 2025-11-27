#include "Compiler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

#include "CompilerException.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return EXIT_FAILURE;
    }

    const std::string inputFilePath = argv[1];

    try {
        const Compiler compiler(inputFilePath);
        compiler.compile();

        return EXIT_SUCCESS;
    } catch (const CompilerException &e) {
        std::cerr << "Error during compilation:\n";
        for (const auto &error: e.errors()) {
            std::cerr << std::format("{}:{}", inputFilePath, error);
        }
        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << "Compiler error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
