#ifndef CPPLISP_LISP_H
#define CPPLISP_LISP_H

#include <string>
#include <variant>
#include <vector>

using Number = float;
using Symbol = std::string;

struct Nil {
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
};

struct Lambda {
    std::vector<Symbol> arguments;
    Expression body;
};

using Result = std::variant<Nil, Number, Symbol, Lambda, bool>;

std::string to_string(Result res);

Result eval_program(std::string program);

#endif //CPPLISP_LISP_H
