#include "lisp.h"

int main() {
//    std::string program = "(+ (- 0 1 9) (+ 2 2 3 4))";
//    std::string program = "(let (x 1 y 2) (+ x (* 1 y)))";
    std::string program = "(print 1)";
//    std::string program = "(do (print 1) (print 2))";
//    std::string program = "(do (define x 1) (+ x 1))";

    auto tokens = tokenize(program);

    auto[ast, _] = parse(tokens);

    Env env;
    std::cout << std::get<Number>(eval(ast, env)) << std::endl;

    return 0;
}

/*
 * (+ 1 (+ 1 2 3))
 *
 * (let (x 1 y 2) (+ x y))
 */