#include "parser.h"
#include "utility.h"
#include <iostream>

ExprPtr atom(const std::string &token) {
  try {
    return std::make_shared<LiteralExpr<Number>>(std::stod(token));
  } catch (std::invalid_argument &) {
    return std::make_shared<SymbolExpr>(Symbol{token});
  }
}

std::shared_ptr<LetExpr> parse_let(const ExprList &expressions) {
  if (expressions.size() < 2) {
    throw SyntaxError("Requires two arguments for 'let'");
  }

  auto vars = dynamic_cast<ListExpr *>(expressions[0].get());
  if (vars == nullptr) {
    throw SyntaxError("First argument of 'let' should be a list");
  }

  LetExpr::Bindings bindings;
  for (int i = 0; i < vars->expressions.size(); i += 2) {
    auto symbol = dynamic_cast<SymbolExpr *>(vars->expressions[i].get());
    if (symbol == nullptr) {
      throw SyntaxError("Not a symbol");
    }

    if (i >= vars->expressions.size() - 1) {
      throw SyntaxError("No value after 'let' variable: " +
                        symbol->symbol.name);
    }
    bindings.emplace_back(symbol->symbol, vars->expressions[i + 1]);
  }

  auto body = expressions[1];
  return std::make_shared<LetExpr>(bindings, body);
}

std::shared_ptr<DefineExpr> parse_define(const ExprList &expressions) {
  if (expressions.size() < 2) {
    throw std::runtime_error("Expected two arguments after 'define'");
  }
  auto var = dynamic_cast<SymbolExpr *>(expressions[0].get());
  if (var == nullptr) {
    throw SyntaxError("Not a symbol");
  }
  auto expr = expressions[1];
  return std::make_shared<DefineExpr>(*var, expr);
}

std::shared_ptr<LambdaExpr> parse_lambda(const ExprList &expressions) {
  if (expressions.size() < 2) {
    throw SyntaxError("Expected two arguments for 'lambda'");
  }
  auto args = dynamic_cast<ListExpr *>(expressions[0].get());
  if (args == nullptr) {
    throw SyntaxError("Expected list of arguments");
  }
  return std::make_shared<LambdaExpr>(*args, expressions[1]);
}

ExprPtr parse_language_construct(SymbolExpr *s, const ExprList &expressions) {
  if (s->symbol.name == "do") {
    return std::make_shared<DoExpr>(rest(expressions));

  } else if (s->symbol.name == "if") {
    return std::make_shared<IfExpr>(rest(expressions));

  } else if (s->symbol.name == "define") {
    return parse_define(rest(expressions));

  } else if (s->symbol.name == "let") {
    return parse_let(rest(expressions));

  } else if (s->symbol.name == "lambda") {
    return parse_lambda(rest(expressions));

  } else if (s->symbol.name == "cond") {
    return std::make_shared<CondExpr>(rest(expressions));

  } else if (s->symbol.name == "and") {
    return std::make_shared<AndExpr>(rest(expressions));

  } else if (s->symbol.name == "or") {
    return std::make_shared<OrExpr>(rest(expressions));

  } else {
    return std::make_shared<ListExpr>(expressions);
  }
}

ExprPtr Parser::parse(const Tokens &tokens) {
  while (current < tokens.size()) {
    auto token = tokens[current];
    if (token.type == LEFTPAREN) {
      current++;
      if (current >= tokens.size()) {
        throw IncompleteStatement("Expected expression after '('");
      }

      ExprList expressions;
      while (tokens[current].type != RIGHTPAREN) {
        expressions.push_back(parse(tokens));

        if (current >= tokens.size()) {
          throw IncompleteStatement("Expected ')'");
        }
      }

      // go past the RIGHTPAREN
      current++;

      if (expressions.empty()) {
        return std::make_shared<ListExpr>();
      } else {
        auto s = dynamic_cast<SymbolExpr *>(expressions[0].get());
        if (s != nullptr) {
          return parse_language_construct(s, expressions);
        } else {
          return std::make_shared<ListExpr>(expressions);
        }
      }

    } else if (token.type == RIGHTPAREN) {
      throw std::runtime_error("Unexpected ')'");

    } else if (token.type == STRING) {
      current++;
      return std::make_shared<LiteralExpr<String>>(token.val);

    } else {
      current++;
      return atom(token.val);
    }
  }
}

ExprList Parser::parse_all(const Tokens &tokens) {
  ExprList exprs;
  while (current < tokens.size()) {
    exprs.push_back(parse(tokens));
  }
  return exprs;
}
