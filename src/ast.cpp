#include "ast.h"

#include "builtins.h"

std::vector<Result> eval_all(Env &env, const ExprList &exprs) {
  std::vector<Result> results;
  results.reserve(exprs.size());
  for (const auto &expr : exprs) {
    results.push_back(expr->evaluate(env));
  }
  return results;
}

Result apply_lambda(const Lambda &lambda, Env &env,
                    const std::vector<Result> &args) {
  Env bindings = env;
  for (int i = 0; i < args.size(); ++i) {
    bindings[lambda.arguments[i].name] = args[i];
  }

  bindings.merge(*lambda.env);
  return lambda.body->evaluate(bindings);
}

Result SymbolExpr::evaluate(Env &env) {
  if (auto val = env.get(symbol.name)) {
    return *val;
  } else {
    throw std::runtime_error("Undeclared symbol " + symbol.name);
  }
}

Result ListExpr::evaluate(Env &env) {
  if (expressions.empty()) {
    return List();
  }

  auto expr = dynamic_cast<SymbolExpr *>(expressions[0].get());
  if (expr != nullptr) {
    if (auto func = env.get(expr->symbol.name)) {
      if (auto lambda = std::get_if<Lambda>(func)) {
        return apply_lambda(*lambda, env, eval_all(env, rest(expressions)));
      } else {
        throw std::runtime_error("Cannot apply, not a function: " +
                                 to_string(*func));
      }
    } else {
      return apply_op(expr->symbol.name, eval_all(env, rest(expressions)));
    }
  } else {
    auto obj = expressions[0]->evaluate(env);
    if (auto lambda = std::get_if<Lambda>(&obj)) {
      return apply_lambda(*lambda, env, eval_all(env, rest(expressions)));
    } else {
      throw std::runtime_error("Unknown operator");
    }
  }
}

Result DoExpr::evaluate(Env &env) {
  // keep only last result, discard the rest for optimisation
  Result res;
  for (auto &expr : expressions) {
    res = expr->evaluate(env);
  }
  return res;
}

Result IfExpr::evaluate(Env &env) {
  if (is_true(expressions[0]->evaluate(env))) {
    return expressions[1]->evaluate(env);
  } else {
    return expressions[2]->evaluate(env);
  }
}

Result CondExpr::evaluate(Env &env) {
  for (const auto &expr : expressions) {
    auto pair = dynamic_cast<ListExpr *>(expr.get());
    if (pair == nullptr) {
      throw std::runtime_error("Clause is not a pair of expressions");
    }
    if (pair->expressions.size() < 2) {
      throw std::runtime_error("Require two expressions per clause in 'cond'");
    }

    auto cond_else = dynamic_cast<SymbolExpr *>(pair->expressions[0].get());
    if (cond_else != nullptr && cond_else->symbol.name == "else") {
      return pair->expressions[1]->evaluate(env);
    }

    auto condition = pair->expressions[0]->evaluate(env);
    if (is_true(condition)) {
      return pair->expressions[1]->evaluate(env);
    }
  }

  return Nil{};
}

Result DefineExpr::evaluate(Env &env) {
  env[var.symbol.name] = expr->evaluate(env);
  return Nil{};
}

Result LetExpr::evaluate(Env &env) {
  Env new_env = env;
  for (const auto &pair : vars) {
    // passing new_env to evaluate() make the previous bindings available to
    // further declarations, e.g.: (let (x 1 y (+ x 2)) y) -> 3
    new_env[pair.first.name] = pair.second->evaluate(new_env);
  }

  return expr->evaluate(new_env);
}

Result LambdaExpr::evaluate(Env &env) {
  std::vector<Symbol> args;
  for (const auto &arg : arguments.expressions) {
    auto symbol = dynamic_cast<SymbolExpr *>(arg.get());
    if (symbol == nullptr) {
      throw SyntaxError("Lambda argument is not a symbol");
    }
    args.push_back(symbol->symbol);
  }

  // copy env to pointer
  auto closure_env = std::make_shared<Env>();
  *closure_env = env;
  return Lambda{args, body, closure_env};
}

Result AndExpr::evaluate(Env &env) {
  for (const auto &expr : exprs) {
    if (!is_true(expr->evaluate(env))) {
      return false;
    }
  }
  return true;
}

Result OrExpr::evaluate(Env &env) {
  for (const auto &expr : exprs) {
    if (is_true(expr->evaluate(env))) {
      return true;
    }
  }
  return false;
}
