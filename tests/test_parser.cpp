#include "catch.hpp"

#include "../src/tokenizer.h"
#include "../src/parser.h"

TEST_CASE("basic parser") {
  auto expr = Parser().parse(tokenize("()"));
  auto e = dynamic_cast<ListExpr*>(expr.get());
  REQUIRE(e != nullptr);
  REQUIRE(e->expressions.empty());
}


TEST_CASE("op parser") {
  auto expr = Parser().parse(tokenize("(+ 1 2)"));
  auto e = dynamic_cast<ListExpr*>(expr.get());
  REQUIRE(e != nullptr);
  REQUIRE(e->expressions.size() == 3);
}

TEST_CASE("lambda parser") {
  auto expr = Parser().parse(tokenize("(lambda (x) x)"));
  auto e = dynamic_cast<LambdaExpr*>(expr.get());
  REQUIRE(e != nullptr);
  REQUIRE(e->arguments.expressions.size() == 1);
  Env env;
  auto lambda = e->evaluate(env);
  REQUIRE(std::holds_alternative<Lambda>(lambda));
}