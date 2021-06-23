#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

class SyntaxError : public std::runtime_error {
public:
  explicit SyntaxError(const std::string &msg)
      : std::runtime_error(msg.c_str()) {}
};

class Expr;

struct Symbol {
  std::string name;
};

struct Nil {};

using Boolean = bool;
using String = std::string;
using Number = double;

struct List;
struct Lambda;
using Result = std::variant<Nil, Number, Lambda, Boolean, List, String, Symbol>;

class Env;
struct Lambda {
  std::vector<Symbol> arguments;
  std::shared_ptr<Expr> body;
  std::shared_ptr<Env> env;
};

struct List {
  std::vector<Result> list;

  List() = default;

  explicit List(std::vector<Result> l) : list(std::move(l)) {}
};

std::string to_string(Result res);