#ifndef CPPLISP_LISP_H
#define CPPLISP_LISP_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

std::string stdlib();

using Tokens = std::vector<std::string>;

Tokens tokenize(std::string program);


using Number = double;
using Symbol = std::string;

struct Nil {
};

class IncompleteStatement : public std::runtime_error {
public:
    explicit IncompleteStatement(std::string msg) : std::runtime_error(msg.c_str()) {}
};


struct Expression {
    using List = std::vector<Expression>;
    std::variant<Number, Symbol, List> value;

    Expression() {}

    Expression(Number n) : value(n) {}

    Expression(Symbol s) : value(std::move(s)) {}

    Expression(List l) : value(std::move(l)) {}

    Number get_number() {
        return std::get<Number>(value);
    }

    Symbol get_symbol() {
        return std::get<Symbol>(value);
    }

    List get_list() {
        return std::get<List>(value);
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
};

std::pair<Expression, int> parse(Tokens tokens, int start = 0);

std::vector<Expression> parse_all(Tokens tokens);


struct Lambda {
    std::vector<Symbol> arguments;
    Expression body;
};

struct List;
using Result = std::variant<Nil, Number, Symbol, Lambda, bool, List>;
struct List {
    std::vector<Result> list;
};

std::string to_string(Result res);

using Env = std::unordered_map<std::string, Result>;

std::pair<Result, Env> eval_with_env(std::string program, Env env);

Result eval_program(std::string program);

#endif //CPPLISP_LISP_H
