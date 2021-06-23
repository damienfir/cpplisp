#pragma once

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "tokenizer.h"
#include "ast.h"

class IncompleteStatement : public std::runtime_error {
public:
  explicit IncompleteStatement(const std::string& msg)
      : std::runtime_error(msg.c_str()) {}
};



class Parser {
private:
  int current;

public:
  Parser() : current(0) {}

  ExprPtr parse(const Tokens &tokens);
  std::vector<ExprPtr> parse_all(const Tokens &tokens);
};
