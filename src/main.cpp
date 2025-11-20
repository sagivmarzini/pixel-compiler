#include "Compiler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <input_file>\n";
            return EXIT_FAILURE;
        }

        const std::string inputPath = argv[1];

        // Open and validate input file
        std::ifstream file(inputPath);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open file '" << inputPath << "'\n";
            return EXIT_FAILURE;
        }

        // Read source code from file
        std::ostringstream buffer;
        buffer << file.rdbuf();
        file.close();

        const std::string sourceCode = buffer.str();

        // Initialize and run compiler
        Compiler compiler(sourceCode);
        compiler.compile();

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception occurred\n";
        return EXIT_FAILURE;
    }
}
