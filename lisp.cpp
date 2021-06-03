#include <iostream>
#include <utility>
#include <vector>
#include <variant>
#include <tuple>
#include <memory>
#include <unordered_map>
#include <set>

#include "lisp.h"

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

bool equals_fn(const std::vector<Result> &arguments) {
    if (arguments.size() != 2) {
        throw std::runtime_error("'=' operator requires exactly 2 arguments");
    }

    return std::get<float>(arguments[0]) == std::get<float>(arguments[1]);
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
    } else if (op == "=") {
        return equals_fn(arguments);
    } else {
        throw std::runtime_error("Unknown operation: " + op);
    }
}

using Env = std::unordered_map<std::string, Result>;

std::pair<Result, Env> eval(Expression expr, Env env);

Env make_env(Expression::List vars, Env env) {
    Env new_env;
    for (int i = 0; i < vars.size(); i += 2) {
        auto key = vars[i].get_symbol();
        auto val = eval(vars[i + 1], env).first;
        new_env[key] = val;
    }
    return new_env;
}

Env merge(Env a, Env b) {
    // modifies both a and b but it's ok because we get copies as arguments
    a.merge(b);
    return a;
}


std::string to_string(Result res) {
    if (auto n = std::get_if<Number>(&res)) {
        return std::to_string(*n);
    } else if (std::get_if<Nil>(&res)) {
        return "nil";
    } else if (auto s = std::get_if<Symbol>(&res)) {
        return *s;
    } else if (auto b = std::get_if<bool>(&res)) {
        return (*b) ? "true" : "false";
    } else if (auto l = std::get_if<List>(&res)) {
        auto list = *l;
        std::string s = "(";
        for (const auto& v : list.list) {
            s += to_string(v) + " ";
        }
        s += ")";
        return s;
    }
}

std::set<std::string> builtins = {
        "+", "-", "/", "*", "let", "println", "do", "define", "lambda", "if", "=", "list", "first", "rest"
};

std::pair<Result, Env> eval_builtin(Symbol op, Expression::List list, Env env) {
    if (op == "let") {
        auto new_env = make_env(list[1].get_list(), env);
        return {eval(list[2], merge(env, new_env)).first, env};

    } else if (op == "println") {
        auto res = eval(list[1], env).first;
        std::cout << to_string(res) << std::endl;
        return {Nil{}, env};

    } else if (op == "do") {
        auto new_env = env;
        for (int i = 1; i < list.size() - 1; ++i) {
            new_env = eval(list[i], new_env).second;
        }

        // Do not return new environment
        return {eval(list[list.size() - 1], new_env).first, env};

    } else if (op == "define") {
        auto key = std::get<Symbol>(list[1].value);
        auto val = eval(list[2], env).first;
        env[key] = val;
        // return modified environment
        return {Nil{}, env};

    } else if (op == "lambda") {
        auto arguments = list[1].get_list();
        std::vector<Symbol> args;
        args.reserve(arguments.size());
        for (auto arg : arguments) {
            args.push_back(arg.get_symbol());
        }

        return {Lambda{args, list[2]}, env};

    } else if (op == "if") {
        if (list.size() != 4) {
            throw std::runtime_error("Wrong syntax for 'if'");
        }
        auto res = eval(list[1], env).first;
        if (std::get<bool>(res)) {
            return {eval(list[2], env).first, env};
        } else {
            return {eval(list[3], env).first, env};
        }

    } else if (op == "list") {
        List res;
        for (int i = 1; i < list.size(); ++i) {
            res.list.push_back(eval(list[i], env).first);
        }
        return {res, env};

    } else {
        std::vector<Result> arguments;
        for (int i = 1; i < list.size(); ++i) {
            arguments.push_back(eval(list[i], env).first);
        }
        return {apply_op(op, arguments), env};
    }
}

std::pair<Result, Env> eval(Expression expr, Env env) {
    if (auto n = std::get_if<Number>(&expr.value)) {
        return {*n, env};

    } else if (auto s = std::get_if<Symbol>(&expr.value)) {
        if (builtins.contains(*s)) {
            return {*s, env};
        } else if (env.contains(*s)) {
            return {env.at(*s), env};
        } else {
            throw std::runtime_error("Undeclared symbol " + *s);
        }

    } else if (auto l = std::get_if<Expression::List>(&expr.value)) {
        auto list = *l;
        auto first = eval(list[0], env).first;

        if (auto op = std::get_if<Symbol>(&first)) {
            return eval_builtin(*op, list, env);

        } else if (auto lambda = std::get_if<Lambda>(&first)) {
            Env bindings = env;
            for (int i = 1; i < list.size(); ++i) {
                bindings[lambda->arguments[i - 1]] = eval(list[i], env).first;
            }

            return {eval(lambda->body, bindings).first, env};
        } else {
            throw std::runtime_error("Unknown operator: " + to_string(first));
        }
    }
}

Result eval_program(std::string program) {
    auto[ast, _] = parse(tokenize(program));
    return eval(ast, Env{}).first;
}

/*
 * lists
 * strings
 * recursion
 * comments
 * standard library
 * abstract common constructs like 'if' that can be implemented with 'cond'
 * repl
 */