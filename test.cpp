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

template<typename T>
bool equals(const std::vector<T> &a, const std::vector<T> &b) {
    return (a.size() == b.size()) && std::equal(std::begin(a), std::end(a), std::begin(b));
}

void test_tokenize() {
    auto tokens = tokenize("(+ 1 2)");
    assert(equals(tokens, std::vector<std::string>({"(", "+", "1", "2", ")"})));

    tokens = tokenize("1");
    assert(tokens[0] == "1");
}

void test_parse() {
    auto tokens = tokenize("1");
    auto[ast, _] = parse(tokens);
    assert(ast.get_number() == 1);
}

int main() {
    test_tokenize();

    test_parse();

    try {
        test("only '('", "(", [](auto res) {});
    } catch (std::runtime_error &e) {
        std::cout << e.what() << "\n\n";
    }

    test("only '()'", "()", [](auto res) {
        assert(std::holds_alternative<Nil>(res));
    });

    test("nested arithmetic", "(+ (- 0 1 2) (+ 1 9 10))", [](auto res) {
        assert(std::get<Number>(res) == 17);
    });

    test("let", "(let (x 1 y 2) (+ x (* 1 y)))", [](auto res) {
        assert(std::get<Number>(res) == 3);
    });

    test("println", "(println 1)", [](auto res) {
        assert(std::holds_alternative<Nil>(res));
    });

    test("do", "(do (println 1) (println 2))", [](auto n) {
        assert(std::holds_alternative<Nil>(n));
    });

    test("define", "(do (define x 1) (define y 2) (+ x y))", [](auto n) {
        assert(std::get<Number>(n) == 3);
    });

    test("lambda", "((lambda (x) (+ x 1)) 1)", [](auto n) {
        assert(std::get<Number>(n) == 2);
    });

    test("if", "(if (= 1 1) 1 2)", [](auto n) {
        assert(std::get<Number>(n) == 1);
    });

    test("if 2", "(if (= 1 2) 1 2)", [](auto n) {
        assert(std::get<Number>(n) == 2);
    });

    test("equals", "(do (println 1) (println (= 1 1)) (println (= 1 2)))", [](auto res) {
        assert(std::holds_alternative<Nil>(res));
    });

    test("list", "(list 1 2 3)", [](auto res) {
        auto l = std::get<List>(res).list;
        assert(std::get<Number>(l[0]) == 1);
        assert(std::get<Number>(l[1]) == 2);
        assert(std::get<Number>(l[2]) == 3);
    });

    test("list first", "(first (list 1 2 3))", [](auto res) {
        assert(std::get<Number>(res) == 1);
    });

    test("list rest", "(rest (list 1 2 3))", [](auto res) {
        auto l = std::get<List>(res).list;
        assert(std::get<Number>(l[0]) == 2);
        assert(std::get<Number>(l[1]) == 3);
    });

    test("recursion", "(do"
                      "(define fn (lambda (x)"
                      "(do"
                      "(println x)"
                      "(if (= x 1)"
                      "1"
                      "(+ x (fn (- x 1)))))))"
                      "(fn 10))", [](auto res) {
        assert(std::get<Number>(res) == 55);
    });

    test("factorial", "(do "
                      "(define factorial (lambda (n) "
                        "(if (= n 0)"
                            "1 "
                            "(* n (factorial (- n 1))))))"
                    "(factorial 100))", [](auto res) {
//        assert(std::get<Number>(res) == 3628800);
    });
}