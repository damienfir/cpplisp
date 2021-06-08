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