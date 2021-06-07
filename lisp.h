#ifndef CPPLISP_LISP_H
#define CPPLISP_LISP_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

std::string stdlib();

enum TokenType {
    LEFTPAREN,
    RIGHTPAREN,
    STRING,
    SYMBOL
};

struct Token {
    TokenType type;
    std::string val;

    explicit Token(TokenType type) : type(type) {}

    Token(TokenType type, std::string value) : type(type), val(value) {}
};

using Tokens = std::vector<Token>;

Tokens tokenize(const std::string &program);

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

struct Nil {
};

struct Lambda {
    std::vector<Symbol> arguments;
    Expression body;
};

struct List;
using Result = std::variant<Nil, Number, Lambda, bool, List, String>;
struct List {
    std::vector<Result> list;
};

std::string to_string(Result res);

using Env = std::unordered_map<std::string, Result>;

std::pair<Result, Env> eval_with_env(std::string program, Env env);

Result eval_program(std::string program);

#endif //CPPLISP_LISP_H
