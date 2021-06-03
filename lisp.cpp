#include <iostream>
#include <utility>
#include <vector>
#include <variant>
#include <tuple>
#include <memory>
#include <unordered_map>

#include "lisp.h"

template<typename T>
void print(const std::vector<T> &v) {
    for (const auto &s : v) std::cout << s << std::endl;
}

using Tokens = std::vector<std::string>;

Tokens tokenize(std::string program) {
    Tokens tokens;
    std::string tmp;
    for (auto c : program) {
        if (c == ' ') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp = "";
            }
        } else if (c == '(' || c == ')') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp = "";
            }
            tokens.emplace_back(1, c);
        } else {
            tmp += c;
        }
    }
    return tokens;
}


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

void print_n_spaces(int n) {
    for (int i = 0; i < n * 2; ++i) {
        std::cout << " ";
    }
}

void print_ast(const Expression &ast, int inc = 0) {
    if (auto n = std::get_if<Number>(&ast.value)) {
        print_n_spaces(inc);
        std::cout << *n << std::endl;
    } else if (auto s = std::get_if<Symbol>(&ast.value)) {
        print_n_spaces(inc);
        std::cout << *s << std::endl;
    } else if (auto l = std::get_if<Expression::List>(&ast.value)) {
        for (auto e : *l) {
            print_ast(e, inc + 1);
        }
    }
}

Expression atom(std::string token) {
    try {
        return std::stof(token);
    } catch (std::invalid_argument &) {
        return token;
    }
}

std::tuple<Expression, int> parse(Tokens tokens, int start = 0) {
    int i = start;
    while (i <= tokens.size()) {
        if (tokens[i] == "(") {
            i++;
            std::vector<Expression> exprs;
            while (tokens[i] != ")") {
                Expression expr;
                std::tie(expr, i) = parse(tokens, i);
                exprs.push_back(expr);
            }
            return {exprs, i + 1};
        } else if (tokens[i] == ")") {
            throw std::runtime_error("Unexpected ')'");
        } else {
            return {atom(tokens[i]), i + 1};
        }
    }
}


float plus_fn(const std::vector<Result> &arguments) {
    float n = 0;
    for (auto a : arguments) {
        n += std::get<float>(a);
    }
    return n;
}

float minus_fn(const std::vector<Result> &arguments) {
    float n = std::get<float>(arguments[0]);
    for (int i = 1; i < arguments.size(); ++i) {
        n -= std::get<float>(arguments[i]);
    }
    return n;
}

float divide_fn(const std::vector<Result> &arguments) {
    float n = std::get<float>(arguments[0]);
    for (int i = 1; i < arguments.size(); ++i) {
        n /= std::get<float>(arguments[i]);
    }
    return n;
}

float multiply_fn(const std::vector<Result> &arguments) {
    float n = std::get<float>(arguments[0]);
    for (int i = 1; i < arguments.size(); ++i) {
        n *= std::get<float>(arguments[i]);
    }
    return n;
}

Result apply_op(const std::string &op, const std::vector<Result> &arguments) {
    if (op == "+") {
        return plus_fn(arguments);
    } else if (op == "-") {
        return minus_fn(arguments);
    } else if (op == "/") {
        return divide_fn(arguments);
    } else if (op == "*") {
        return multiply_fn(arguments);
    } else {
        throw std::runtime_error("Unknown operation: " + op);
    }
}

using Env = std::unordered_map<std::string, Number>;

Env make_env(Expression::List vars) {
    Env env;
    for (int i = 0; i < vars.size(); i += 2) {
        auto key = std::get<Symbol>(vars[i].value);
        auto val = std::get<Number>(vars[i + 1].value);
        env[key] = val;
    }
    return env;
}

Env merge(Env a, Env b) {
    // modifies both a and b but it's ok because we get copies as arguments
    a.merge(b);
    return a;
}

//Result eval(Expression expr, const Env &env);

//std::vector<Result> eval_all(Expression::List exprs, const Env &env) {
//    std::vector<Result> arguments;
//    for (auto e : exprs) {
//        arguments.push_back(eval(e, env));
//    }
//    return arguments;
//}

std::pair<Result, Env> eval(Expression expr, Env env) {
    if (auto n = std::get_if<Number>(&expr.value)) {
        return {*n, env};

    } else if (auto s = std::get_if<Symbol>(&expr.value)) {
        if (env.contains(*s)) {
            return {env.at(*s), env};
        } else {
            throw std::runtime_error("Undeclared symbol " + *s);
        }

    } else if (auto l = std::get_if<Expression::List>(&expr.value)) {
        auto list = *l;
        auto op = list[0].get_symbol();

        if (op == "let") {
            auto new_env = make_env(list[1].get_list());
            return {eval(list[2], merge(env, new_env)).first, env};

        } else if (op == "println") {
            auto res = eval(list[1], env).first;
            std::cout << std::get<float>(res) << std::endl;
            return {0.f, env};

        } else if (op == "do") {
            auto new_env = env;
            for (int i = 1; i < list.size() - 1; ++i) {
                new_env = eval(list[i], new_env).second;
            }

            // Do not return new environment
            return {eval(list[list.size() - 1], new_env).first, env};

        } else if (op == "define") {
            auto key = std::get<Symbol>(list[1].value);
            auto val = std::get<Number>(eval(list[2], env).first);
            env[key] = val;

            // return modified environment
            return {0.f, env};
        } else if (op == "lambda") {
            return {0.f, env};

        } else {

            std::vector<Result> arguments;
            for (int i = 1; i < list.size(); ++i) {
                arguments.push_back(eval(list[i], env).first);
            }
            return {apply_op(op, arguments), env};
        }
    }
}

Result eval_program(std::string program) {
    auto[ast, _] = parse(tokenize(program));
    return eval(ast, Env{}).first;
}

