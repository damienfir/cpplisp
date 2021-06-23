#include <iostream>
#include <memory>
#include <set>
#include <tuple>
#include <utility>

#include "lisp.h"

std::string stdlib() {
  return R"stdlib(
(define empty? (lambda (seq)
  (= (length seq) 0)))

(define map (lambda (fn seq)
  (if (empty? seq)
      (list)
      (cons (fn (first seq)) (map fn (rest seq))))))
)stdlib";
}

Result eval_all(const ExprList &exprs, Env &env) {
  Result res;
  for (auto &expr : exprs) {
    res = expr->evaluate(env);
  }
  return res;
}

Result eval_with_env(const std::string &program, Env &env) {
  auto ast = Parser().parse_all(tokenize(program));
  return eval_all(ast, env);
}

Result eval_program(const std::string &program) {
  Env env;
  return eval_with_env(program, env);
}

Result eval_program_with_stdlib(const std::string &program) {
  Env env;
  eval_with_env(stdlib(), env);
  return eval_with_env(program, env);
}

/*
 * nested environments
 * closures
 */