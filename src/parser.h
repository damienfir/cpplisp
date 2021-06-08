#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <variant>

#include "tokenizer.h"

using Number = double;
using Symbol = std::string;

class IncompleteStatement : public std::runtime_error {
public:
    explicit IncompleteStatement(std::string msg) : std::runtime_error(msg.c_str()) {}
};


struct String {
    std::string value;
};

struct Expression {
    using List = std::vector<Expression>;
    std::variant<Number, Symbol, List, String> value;

    Expression() {}

    Expression(Number n) : value(n) {}

    Expression(Symbol s) : value(std::move(s)) {}

    Expression(List l) : value(std::move(l)) {}

    Expression(String s) : value(std::move(s)) {}

    Number get_number() {
        return std::get<Number>(value);
    }

    Symbol get_symbol() {
        return std::get<Symbol>(value);
    }

    List get_list() {
        return std::get<List>(value);
    }

    String get_string() {
        return std::get<String>(value);
    }

    bool is_symbol() {
        return std::holds_alternative<Symbol>(value);
    }

    bool is_number() {
        return std::holds_alternative<Number>(value);
    }

    bool is_list() {
        return std::holds_alternative<List>(value);
    }

    bool is_string() {
        return std::holds_alternative<String>(value);
    }
};

std::pair<Expression, int> parse(Tokens tokens, int start = 0);

std::vector<Expression> parse_all(Tokens tokens);
