#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"

#include "../src/lisp.h"

TEST_CASE("Nested arithmetic") {
    auto res = eval_program("(+ (- 0 1 2) (+ 1 9 10))");
    REQUIRE(std::get<Number>(res) == 17);
}

TEST_CASE("Let") {
    auto res = eval_program("(let (x 1 y 2) (+ x (* 1 y)))");
    REQUIRE(std::get<Number>(res) == 3);
}

TEST_CASE("println") {
    auto res = eval_program("(println 1)");
    REQUIRE(std::holds_alternative<Nil>(res));
}

TEST_CASE("do") {
    auto res = eval_program("(do (println 1) (println 2))");
    REQUIRE(std::holds_alternative<Nil>(res));
}

TEST_CASE("define") {
    auto res = eval_program("(do (define x 1) (define y 2) (+ x y))");
    REQUIRE(std::get<Number>(res) == 3);
}

TEST_CASE("lambda") {
    auto res = eval_program("((lambda (x) (+ x 1)) 1)");
    REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("if") {
    auto res = eval_program("(if (= 1 1) 1 2)");
    REQUIRE(std::get<Number>(res) == 1);
}

TEST_CASE("if 2") {
    auto res = eval_program("(if (= 1 2) 1 2)");
    REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("equals") {
    auto res = eval_program("(do (println 1) (println (= 1 1)) (println (= 1 2)))");
    REQUIRE(std::holds_alternative<Nil>(res));
}

TEST_CASE("lt") {
    auto res = eval_program("(< 1 2)");
    REQUIRE(std::get<bool>(res));
}

TEST_CASE("gt") {
    auto res = eval_program("(> 3 2)");
    REQUIRE(std::get<bool>(res));
}

TEST_CASE("gte") {
    auto res = eval_program("(>= 2 2)");
    REQUIRE(std::get<bool>(res));
}

TEST_CASE("lte") {
    auto res = eval_program("(<= 2 2)");
    REQUIRE(std::get<bool>(res));
}

TEST_CASE("list") {
    auto res = eval_program("(list 1 2 3)");
    auto l = std::get<List>(res).list;
    REQUIRE(std::get<Number>(l[0]) == 1);
    REQUIRE(std::get<Number>(l[1]) == 2);
    REQUIRE(std::get<Number>(l[2]) == 3);
}

TEST_CASE("list first") {
    auto res = eval_program("(first (list 1 2 3))");
    REQUIRE(std::get<Number>(res) == 1);
}

TEST_CASE("list rest") {
    auto res = eval_program("(rest (list 1 2 3))");
    auto l = std::get<List>(res).list;
    REQUIRE(std::get<Number>(l[0]) == 2);
    REQUIRE(std::get<Number>(l[1]) == 3);
}

TEST_CASE("recursion") {
    auto res = eval_program(R"lisp(
   (define fn (lambda (x)
       (do
           (println x)
               (if (= x 1)
               1
               (+ x (fn (- x 1)))))))
   (fn 10))lisp");
    REQUIRE(std::get<Number>(res) == 55);
}

TEST_CASE("factorial") {
    auto res = eval_program(R"lisp(
(define factorial (lambda (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1))))))
(factorial 10))lisp");
    REQUIRE(std::get<Number>(res) == 3628800);
}

TEST_CASE("multi-instruction") {
    auto res = eval_program("(define x 1) (println x) (define y 2) (println y) (+ x y)");
    REQUIRE(std::get<Number>(res) == 3);
}

TEST_CASE("cond") {
    auto res = eval_program("(cond ((= 1 2) 1) ((= 1 1) 2))");
    REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("string") {
    auto res = eval_program("(do (define s \"hello, world!\") (println s) s)");
    REQUIRE(std::get<String>(res).value == "hello, world!");
}

TEST_CASE("comments") {
    auto res = eval_program(R"lisp(
(println "print this")
; this is a comment
(println "print this too")
)lisp");
    REQUIRE(std::holds_alternative<Nil>(res));
}

TEST_CASE("cons") {
    auto res = eval_program("(cons 1 (list 2 3))");
    auto list = std::get<List>(res).list;
    REQUIRE(std::get<Number>(list[0]) == 1);
    REQUIRE(std::get<Number>(list[1]) == 2);
    REQUIRE(std::get<Number>(list[2]) == 3);
}

TEST_CASE("cons empty") {
    auto res = eval_program("(cons 1 (list))");
    auto list = std::get<List>(res).list;
    REQUIRE(list.size() == 1);
    REQUIRE(std::get<Number>(list[0]) == 1); }

TEST_CASE("length 0") {
    auto res = eval_program("(length (list))");
    REQUIRE(std::get<Number>(res) == 0);
}

TEST_CASE("length") {
    auto res = eval_program("(length (list 1 2))");
    REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("stdlib: empty?") {
    auto res = eval_program("(empty? (list))");
    REQUIRE(std::get<bool>(res));
}


TEST_CASE("stdlib: empty? 2") {
    auto res = eval_program("(empty? (list 1 2))");
    REQUIRE(std::get<bool>(res) == false);
}

TEST_CASE("map") {
    auto res = eval_program("(map (lambda (x) (+ x 1)) (list 1 2 3))");
    auto l = std::get<List>(res).list;
    REQUIRE(std::get<Number>(l[0]) == 2);
    REQUIRE(std::get<Number>(l[1]) == 3);
    REQUIRE(std::get<Number>(l[2]) == 4);
}