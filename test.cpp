#include <cassert>
#include <iostream>
#include "lisp.h"

void test0() {
    std::cout << "Test 0:\n";
    std::string program = "(+ (- 0 1 2) (+ 1 9 10))";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::get<float>(n) == 17);
    std::cout << "\n";
}

void test1() {
    std::cout << "Test 1:\n";
    std::string program = "(let (x 1 y 2) (+ x (* 1 y)))";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::get<float>(n) == 3);
    std::cout << "Ok\n\n";
}

void test2() {
    std::cout << "Test 2:\n";
    std::string program = "(println 1)";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::holds_alternative<Nil>(n));
    std::cout << "Ok\n\n";
}

void test3() {
    std::cout << "Test 3:\n";
    std::string program = "(do (println 1) (println 2))";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::holds_alternative<Nil>(n));
    std::cout << "\n";
}

void test4() {
    std::cout << "Test 4:\n";
    std::string program = "(do (define x 1) (define y 2) (+ x y))";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::get<float>(n) == 3);
    std::cout << "\n";
}

void test5() {
    std::cout << "Test 5:\n";
    std::string program = "((lambda (x) (+ x 1)) 1)";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::get<float>(n) == 2);
    std::cout << "\n";
}

void test6() {
    std::cout << "Test 6:\n";
    std::string program = "(if (= 1 1) 1 2)";
    auto n = eval_program(program);
    std::cout << to_string(n) << std::endl;
    assert(std::get<float>(n) == 1);
    std::cout << "\n";
}

void test7() {
    std::cout << "Test 7:\n";
    std::string program = "(if (= 1 2) 1 2)";
    auto n = std::get<float>(eval_program(program));
    std::cout << to_string(n) << std::endl;
    assert(n == 2);
    std::cout << "\n";
}

void test8() {
    std::cout << "Test 8:\n";
    std::string program = "(do (println 1) (println (= 1 1)) (println (= 1 2)))";
    auto res = eval_program(program);
    std::cout << to_string(res) << std::endl;
    assert(std::holds_alternative<Nil>(res));
    std::cout << "\n";
}

void test9() {
    std::cout << "Test 9:\n";
    std::string program = "(println (list 1 2 3))";
    auto res = eval_program(program);
    std::cout << to_string(res) << std::endl;
    assert(std::holds_alternative<Nil>(res));
    std::cout << "\n";
}

int main() {
    test0();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();

    return 1;
}

/*
 * (+ 1 (+ 1 2 3))
 *
 * (let (x 1 y 2) (+ x y))
 */
