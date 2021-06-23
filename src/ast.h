#pragma once

#include "env.h"
#include "types.h"
#include "utility.h"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>

struct Expr {
  virtual Result evaluate(Env &env) = 0;
  //  virtual ~Expr() = default;
};

using ExprPtr = std::shared_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;

std::vector<Result> eval_all(Env &env, const ExprList &exprs);
Result apply_lambda(const Lambda &lambda, Env &env,
                    const std::vector<Result> &args);

struct SymbolExpr : public Expr {
  explicit SymbolExpr(Symbol s) : symbol(std::move(s)) {}
  Result evaluate(Env &env) override;

  Symbol symbol;
};

template <typename T> struct LiteralExpr : public Expr {
  explicit LiteralExpr(T value) : value(std::move(value)) {}

  Result evaluate(Env &env) override { return value; }

  T value;
};

struct ListExpr : public Expr {
  ListExpr() = default;
  explicit ListExpr(ExprList exprs) : expressions(std::move(exprs)) {}
  Result evaluate(Env &env) override;

  ExprList expressions{};
};

struct DoExpr : public Expr {
  explicit DoExpr(ExprList exprs) : expressions(std::move(exprs)) {}
  Result evaluate(Env &env) override;

  ExprList expressions;
};

struct IfExpr : public Expr {
  explicit IfExpr(ExprList exprs) : expressions(std::move(exprs)) {
    if (expressions.size() < 3) {
      throw std::runtime_error("Expected 3 expressions for 'if' statement");
    }
  }
  Result evaluate(Env &env) override;

  ExprList expressions;
};

struct CondExpr : public Expr {
  explicit CondExpr(ExprList exprs) : expressions(std::move(exprs)) {}
  Result evaluate(Env &env) override;

  ExprList expressions;
};

struct DefineExpr : public Expr {
  DefineExpr(SymbolExpr var, ExprPtr expr)
      : var(std::move(var)), expr(std::move(expr)) {}
  Result evaluate(Env &env) override;

  SymbolExpr var;
  ExprPtr expr;
};

struct LetExpr : public Expr {
  using Bindings = std::vector<std::pair<Symbol, ExprPtr>>;

  LetExpr(Bindings vars, ExprPtr expr)
      : vars(std::move(vars)), expr(std::move(expr)) {}
  Result evaluate(Env &env) override;

  Bindings vars;
  ExprPtr expr;
};

struct LambdaExpr : public Expr {
  LambdaExpr(ListExpr args, ExprPtr body)
      : arguments(std::move(args)), body(std::move(body)) {}
  Result evaluate(Env &env) override;

  ListExpr arguments;
  ExprPtr body;
};

struct AndExpr : public Expr {
  explicit AndExpr(ExprList exprs) : exprs(std::move(exprs)) {}
  Result evaluate(Env &env) override;

  ExprList exprs;
};

struct OrExpr : public Expr {
  explicit OrExpr(ExprList exprs) : exprs(std::move(exprs)) {}
  Result evaluate(Env &env) override;

  ExprList exprs;
};