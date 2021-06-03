#include <cassert>
#include <iostream>
#include "lisp.h"

void test0() {
    std::cout << "Test 0:\n";
    std::string program = "(+ (- 0 1 2) (+ 1 9 10))";
    auto n = std::get<float>(eval_program(program));
    assert(n == 17);
    std::cout << "Ok\n\n";
}

void test1() {
    std::cout << "Test 1:\n";
    std::string program = "(let (x 1 y 2) (+ x (* 1 y)))";
    auto n = std::get<float>(eval_program(program));
    assert(n == 3);
    std::cout << "Ok\n\n";
}

void test2() {
    std::cout << "Test 2:\n";
    std::string program = "(println 1)";
    auto n = std::get<float>(eval_program(program));
    assert(n == 0);
    std::cout << "Ok\n\n";
}

void test3() {
    std::cout << "Test 3:\n";
    std::string program = "(do (println 1) (println 2))";
    auto n = std::get<float>(eval_program(program));
    assert(n == 0);
    std::cout << "Ok\n\n";
}

void test4() {
    std::cout << "Test 4:\n";
    std::string program = "(do (define x 1) (define y 2) (+ x y))";
    auto n = std::get<float>(eval_program(program));
    assert(n == 3);
    std::cout << "Ok\n\n";
}

void test5() {
    std::cout << "Test 5:\n";
    std::string program = "((lambda (x) (+ x 1)) 1)";
    auto n = std::get<float>(eval_program(program));
    assert(n == 3);
    std::cout << "Ok\n\n";
}

int main() {
    test0();
    test1();
    test2();
    test3();
    test4();
    test5();

    return 1;
}

/*
 * (+ 1 (+ 1 2 3))
 *
 * (let (x 1 y 2) (+ x y))
 */
