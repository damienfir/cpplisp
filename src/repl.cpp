#include <iostream>
#include <fstream>
#include <sstream>
#include "lisp.h"


int main(int argc, char **argv) {
    if (argc > 1) {
        // Execute a single file
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Cannot open file " << argv[1] << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto output = eval_program_with_stdlib(buffer.str());
        std::cout << to_string(output) << std::endl;
        return 0;
    }

    Env env;
    eval_with_env(stdlib(), env);

    std::string program;
    while (true) {
        std::string line;
        if (program.empty()) {
            std::cout << ">> ";
        } else {
            std::cout << "   ";
        }
        std::getline(std::cin, line);
        program += line + "\n";

        if (line == "exit") {
            return 0;
        }

        try {
            auto res = eval_with_env(program, env);
            std::cout << to_string(res) << std::endl;
            program = "";
        } catch (IncompleteStatement &) {
            // multiline expression
        } catch (std::runtime_error &e) {
            std::cout << e.what() << std::endl;
            program = "";
        }
    }

    return 0;
}