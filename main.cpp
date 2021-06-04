#include <iostream>
#include "lisp.h"

int main() {
    Env env{};

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

        Result res;
        try {
            std::tie(res, env) = eval_with_env(program, env);
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