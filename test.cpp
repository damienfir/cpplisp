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

bool operator==(const Token &a, const Token &b) {
    return a.type == b.type && a.val == b.val;
}

template<typename T>
bool equals(const std::vector<T> &a, const std::vector<T> &b) {
    return (a.size() == b.size()) && std::equal(std::begin(a), std::end(a), std::begin(b));
}

void test_tokenize() {
    std::cout << "test tokenization\n";

    auto tokens = tokenize("(+ 1 2)");
    assert(tokens[0] == Token(LEFTPAREN));
    assert(tokens[1] == Token(SYMBOL, "+"));
    assert(tokens[2] == Token(SYMBOL, "1"));
    assert(tokens[3] == Token(SYMBOL, "2"));
    assert(tokens[4] == Token(RIGHTPAREN));

    tokens = tokenize("1");
    assert(tokens[0].val == "1");

    tokens = tokenize("(x 1) (y 2)");
    assert(equals(tokens,
                  Tokens({Token(LEFTPAREN), Token(SYMBOL, "x"), Token(SYMBOL, "1"), Token(RIGHTPAREN), Token(LEFTPAREN),
                          Token(SYMBOL, "y"), Token(SYMBOL, "2"), Token(RIGHTPAREN)})));

    std::cout << "Ok\n\n";
}

void test_parse() {
    std::cout << "test parsing\n";

    auto tokens = tokenize("1");
    auto ast = parse(tokens).first;
    assert(ast.get_number() == 1);

    tokens = tokenize("(x 1) (y 2)");
    auto asts = parse_all(tokens);
    assert(asts.size() == 2);

    std::cout << "Ok\n\n";
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

    test("lt", "(< 1 2)", [](auto res) {
        assert(std::get<bool>(res));
    });
    test("gt", "(> 3 2)", [](auto res) {
        assert(std::get<bool>(res));
    });
    test("gte", "(>= 2 2)", [](auto res) {
        assert(std::get<bool>(res));
    });
    test("lte", "(<= 2 2)", [](auto res) {
        assert(std::get<bool>(res));
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
                      "(factorial 10))", [](auto res) {
        assert(std::get<Number>(res) == 3628800);
    });

    test("multi-instruction", "(define x 1) (println x) (define y 2) (println y) (+ x y)", [](auto res) {
        assert(std::get<Number>(res) == 3);
    });

    test("cond", "(cond ((= 1 2) 1) ((= 1 1) 2))", [](auto res) {
        assert(std::get<Number>(res) == 2);
    });

    test("string", "(do (define s \"hello, world!\") (println s) s)", [](auto res) {
        assert(std::get<String>(res).value == "hello, world!");
    });

    test("comments", R"lisp(
(println "print this")
; this is a comment
(println "print this too")
)lisp",
         [](auto res) {
             assert(std::holds_alternative<Nil>(res));
         });

    test("cons", "(cons 1 (list 2 3))", [](auto res) {
        auto list = std::get<List>(res).list;
        assert(std::get<Number>(list[0]) == 1);
        assert(std::get<Number>(list[1]) == 2);
        assert(std::get<Number>(list[2]) == 3);
    });

    test("cons empty", "(cons 1 (list))", [](auto res) {
        auto list = std::get<List>(res).list;
        assert(list.size() == 1);
        assert(std::get<Number>(list[0]) == 1);
    });

    test("length 0", "(length (list))", [](auto res) { assert(std::get<Number>(res) == 0); });
    test("length", "(length (list 1 2))", [](auto res) { assert(std::get<Number>(res) == 2); });

    test("stdlib: empty?", "(empty? (list))", [](auto res) { assert(std::get<bool>(res)); });
    test("stdlib: empty? 2", "(empty? (list 1 2))", [](auto res) { assert(std::get<bool>(res) == false); });

    test("map", "(map (lambda (x) (+ x 1)) (list 1 2 3))", [](auto res) {
        auto l = std::get<List>(res).list;
        assert(std::get<Number>(l[0]) == 2);
        assert(std::get<Number>(l[1]) == 3);
        assert(std::get<Number>(l[2]) == 4);
    });
}