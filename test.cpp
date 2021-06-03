#include <cassert>
#include <iostream>
#include "lisp.h"


template<typename Fn>
void test(std::string name, std::string program, Fn assert_fn) {
    std::cout << "Test: " << name << std::endl;
    auto res = eval_program(program);
    std::cout << to_string(res) << std::endl;
    assert_fn(res);
    std::cout << "\n";
}

int main() {
    test("nested arithmetic", "(+ (- 0 1 2) (+ 1 9 10))", [](auto res) {
        assert(std::get<float>(res) == 17);
    });

    test("let", "(let (x 1 y 2) (+ x (* 1 y)))", [](auto res) {
        assert(std::get<float>(res) == 3);
    });

    test("println", "(println 1)", [](auto res) {
        assert(std::holds_alternative<Nil>(res));
    });

    test("do", "(do (println 1) (println 2))", [](auto n) {
        assert(std::holds_alternative<Nil>(n));
    });

    test("define", "(do (define x 1) (define y 2) (+ x y))", [](auto n) {
        assert(std::get<float>(n) == 3);
    });

    test("lambda", "((lambda (x) (+ x 1)) 1)", [](auto n) {
        assert(std::get<float>(n) == 2);
    });

    test("if", "(if (= 1 1) 1 2)", [](auto n) {
        assert(std::get<float>(n) == 1);
    });

    test("if 2", "(if (= 1 2) 1 2)", [](auto n) {
        assert(std::get<float>(n) == 2);
    });

    test("equals", "(do (println 1) (println (= 1 1)) (println (= 1 2)))", [](auto res) {
        assert(std::holds_alternative<Nil>(res));
    });

    test("list", "(list 1 2 3)", [](auto res) {
        auto l = std::get<List>(res).list;
        assert(std::get<float>(l[0]) == 1);
        assert(std::get<float>(l[1]) == 2);
        assert(std::get<float>(l[2]) == 3);
    });

    test("list first", "(first (list 1 2 3))", [](auto res) {
        assert(std::get<float>(res) == 1);
    });

    test("list rest", "(rest (list 1 2 3))", [](auto res) {
        auto l = std::get<List>(res).list;
        assert(std::get<float>(l[0]) == 2);
        assert(std::get<float>(l[1]) == 3);
    });

    test("recursion", "(do"
                      "(define fn (lambda (x)"
                          "(do"
                             "(println x)"
                            "(if (= x 1)"
                                "1"
                                "(+ x (fn (- x 1)))))))"
                      "(fn 10))", [](auto res) {
       assert(std::get<float>(res) == 55);
    });

    return 1;
}