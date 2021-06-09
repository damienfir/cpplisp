#include "catch.hpp"

#include "../src/tokenizer.h"

TEST_CASE("empty program") {
    auto tokens = tokenize("");
    REQUIRE(tokens.empty());
}

TEST_CASE("empty parenthesis") {
    auto tokens = tokenize("()");
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == LEFTPAREN);
    REQUIRE(tokens[1].type == RIGHTPAREN);
}

TEST_CASE("empty nested parenthesis") {
    auto tokens = tokenize("(())");
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == LEFTPAREN);
    REQUIRE(tokens[1].type == LEFTPAREN);
    REQUIRE(tokens[2].type == RIGHTPAREN);
    REQUIRE(tokens[3].type == RIGHTPAREN);
}

TEST_CASE("tokenize strings") {
    auto tokens = tokenize("(println \"hello world\")");
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[2].val == "hello world");
}

TEST_CASE("ignore comments") {
    SECTION("only comments") {
        auto tokens = tokenize("; ignore this");
        REQUIRE(tokens.empty());
    }

    SECTION("after expression") {
        auto tokens = tokenize(("(test) ; ignore"));
        REQUIRE(tokens.size() == 3);
    }

    SECTION("with other expressions") {
        auto tokens = tokenize(R"lisp(
(test)
; ignore comment
(test)
        )lisp");
        REQUIRE(tokens.size() == 6);
    }
}

TEST_CASE("misplaced \" character") {
    REQUIRE_THROWS(tokenize("(te\"st expression)"));
}