#include <iostream>
#include "lisp.h"

int main() {
    Env env{};

    while (true) {
        std::string line;
        std::cout << ">> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            return 0;
        }

        Result res;
        try {
            std::tie(res, env) = eval_with_env(line, env);
            std::cout << to_string(res) << std::endl;
        } catch (std::runtime_error &e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}