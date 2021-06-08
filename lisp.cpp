#include <iostream>
#include <utility>
#include <tuple>
#include <memory>
#include <set>

#include "lisp.h"

std::string stdlib() {
    return R"stdlib(
(define empty? (lambda (seq)
  (= (length seq) 0)))

(define map (lambda (fn seq)
  (if (empty? seq)
      (list)
      (cons (fn (first seq)) (map fn (rest seq))))))
)stdlib";
}

//std::string stdlib() {
//    return R"stdlib(
//)stdlib";
//}


Tokens tokenize(const std::string &program) {
    Tokens tokens;
    std::optional<Token> tmp;
    bool comment = false;

    for (auto c : program) {
        if (c == '\n') {
            if (comment) {
                comment = false;
            }
        } else {
            if (comment) {
                continue;
            }
        }

        if (c == ';') {
            if (!tmp) {
                comment = true;
                continue;
            }
        }

        switch (c) {
            case ' ':
            case '\n':
                if (tmp && tmp->type == STRING) {
                    tmp->val += c;
                } else if (tmp) {
                    tokens.push_back(*tmp);
                    tmp = {};
                }
                break;

            case '(':
                if (tmp && tmp->type == STRING) {
                    tmp->val += c;
                } else if (tmp && tmp->type == SYMBOL) {
                    tokens.push_back(*tmp);
                    tmp = {};
                    tokens.emplace_back(LEFTPAREN);
                } else {
                    tokens.emplace_back(LEFTPAREN);
                }
                break;

            case ')':
                if (tmp && tmp->type == SYMBOL) {
                    tokens.push_back(*tmp);
                    tmp = {};
                    tokens.emplace_back(RIGHTPAREN);
                } else if (tmp && tmp->type == STRING) {
                    tmp->val += c;
                } else {
                    tokens.emplace_back(RIGHTPAREN);
                }
                break;

            case '\"':
                if (tmp && tmp->type == STRING) {
                    tokens.push_back(*tmp);
                    tmp = {};
                } else if (tmp && tmp->type == SYMBOL) {
                    throw std::runtime_error("Character '\"' not allowed in symbol names");
                } else if (!tmp) {
                    tmp = Token(STRING);
                }
                break;

            default:
                if (!tmp) {
                    tmp = Token(SYMBOL, std::string(1, c));
                } else {
                    tmp->val += c;
                }

                break;
        }
    }
    if (tmp) {
        tokens.push_back(*tmp);
    }
    return tokens;
}

//void print_n_spaces(int n) {
//    for (int i = 0; i < n * 2; ++i) {
//        std::cout << " ";
//    }
//}
//
//void print_ast(const Expression &ast, int inc = 0) {
//    if (auto n = std::get_if<Number>(&ast.value)) {
//        print_n_spaces(inc);
//        std::cout << *n << std::endl;
//    } else if (auto s = std::get_if<Symbol>(&ast.value)) {
//        print_n_spaces(inc);
//        std::cout << *s << std::endl;
//    } else if (auto l = std::get_if<Expression::List>(&ast.value)) {
//        for (auto e : *l) {
//            print_ast(e, inc + 1);
//        }
//    }
//}

Expression atom(const std::string &token) {
    try {
        return std::stof(token);
    } catch (std::invalid_argument &) {
        return token;
    }
}

std::pair<Expression, int> parse(Tokens tokens, int start) {
    int i = start;
    while (i <= tokens.size()) {
        auto t = tokens[i];
        if (t.type == LEFTPAREN) {
            i++;
            if (i >= tokens.size()) {
                throw IncompleteStatement("Expected expression after '('");
            }

            std::vector<Expression> exprs;
            while (tokens[i].type != RIGHTPAREN) {
                if (i == tokens.size()) {
                    throw IncompleteStatement("Expected ')'");
                }
                Expression expr;
                std::tie(expr, i) = parse(tokens, i);
                exprs.push_back(expr);
                if (i >= tokens.size()) {
                    throw IncompleteStatement("Expected ')'");
                }
            }
            return {exprs, i + 1};
        } else if (t.type == RIGHTPAREN) {
            throw std::runtime_error("Unexpected ')'");
        } else if (t.type == STRING) {
            return {String{t.val}, i + 1};
        } else {
            return {atom(t.val), i + 1};
        }
    }
}

std::vector<Expression> parse_all(Tokens tokens) {
    std::vector<Expression> exprs;
    int i = 0;
    while (i < tokens.size()) {
        Expression expr;
        std::tie(expr, i) = parse(tokens, i);
        exprs.push_back(expr);
    }
    return exprs;
}

Number plus_fn(const std::vector<Result> &arguments) {
    Number n = 0;
    for (auto a : arguments) {
        n += std::get<Number>(a);
    }
    return n;
}

Number minus_fn(const std::vector<Result> &arguments) {
    Number n = std::get<Number>(arguments[0]);
    for (int i = 1; i < arguments.size(); ++i) {
        n -= std::get<Number>(arguments[i]);
    }
    return n;
}

Number divide_fn(const std::vector<Result> &arguments) {
    Number n = std::get<Number>(arguments[0]);
    for (int i = 1; i < arguments.size(); ++i) {
        n /= std::get<Number>(arguments[i]);
    }
    return n;
}

Number multiply_fn(const std::vector<Result> &arguments) {
    Number n = std::get<Number>(arguments[0]);
    for (int i = 1; i < arguments.size(); ++i) {
        n *= std::get<Number>(arguments[i]);
    }
    return n;
}

void check_one_args(const std::vector<Result> &arguments) {
    if (arguments.size() != 1) {
        throw std::runtime_error("'=' operator requires exactly 1 argument");
    }
}

void check_two_args(const std::vector<Result> &arguments) {
    if (arguments.size() != 2) {
        throw std::runtime_error("'=' operator requires exactly 2 arguments");
    }
}

bool equals_fn(const std::vector<Result> &arguments) {
    check_two_args(arguments);
    return std::get<Number>(arguments[0]) == std::get<Number>(arguments[1]);
}

bool less_than_fn(const std::vector<Result> &arguments) {
    check_two_args(arguments);
    return std::get<Number>(arguments[0]) < std::get<Number>(arguments[1]);
}

bool greater_than_fn(const std::vector<Result> &arguments) {
    check_two_args(arguments);
    return std::get<Number>(arguments[0]) > std::get<Number>(arguments[1]);
}

bool less_than_equals_fn(const std::vector<Result> &arguments) {
    return less_than_fn(arguments) || equals_fn(arguments);
}

bool greater_than_equals_fn(const std::vector<Result> &arguments) {
    return greater_than_fn(arguments) || equals_fn(arguments);
}

Number length_fn(const std::vector<Result> &arguments) {
    check_one_args(arguments);
    if (auto seq = std::get_if<List>(&arguments[0])) {
        return seq->list.size();
    } else {
        throw std::runtime_error("Can only get length of list");
    }
}

List cons_fn(std::vector<Result> arguments) {
    check_two_args(arguments);

    if (auto seq = std::get_if<List>(&arguments[1])) {
        seq->list.insert(seq->list.begin(), arguments[0]);
        return *seq;
    } else {
        throw std::runtime_error("Can only cons to list");
    }
}

List list_fn(std::vector<Result> arguments) {
    return List(std::move(arguments));
}

Result first_fn(const std::vector<Result> &arguments) {
    if (auto seq = std::get_if<List>(&arguments[0])) {
        return seq->list[0];
    } else {
        throw std::runtime_error("Can only get first item from list");
    }
}

List rest_fn(std::vector<Result> arguments) {
    if (auto seq = std::get_if<List>(&arguments[0])) {
        seq->list.erase(seq->list.begin());
        return *seq;
    } else {
        throw std::runtime_error("Can only get the rest of a list");
    }
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
    } else if (op == ">") {
        return greater_than_fn(arguments);
    } else if (op == "<") {
        return less_than_fn(arguments);
    } else if (op == "<=") {
        return less_than_equals_fn(arguments);
    } else if (op == ">=") {
        return greater_than_equals_fn(arguments);
    } else if (op == "length") {
        return length_fn(arguments);
    } else if (op == "cons") {
        return cons_fn(arguments);
    } else if (op == "list") {
        return list_fn(arguments);
    } else if (op == "first") {
        return first_fn(arguments);
    } else if (op == "rest") {
        return rest_fn(arguments);
    } else {
        throw std::runtime_error("Unknown operation: " + op);
    }
}

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

struct PrintVisitor {
    std::string operator()(Number &n) {
        return std::to_string(n);
    }

    std::string operator()(Nil &) {
        return "nil";
    }

    std::string operator()(Symbol &s) {
        return s;
    }

    std::string operator()(bool &b) {
        return b ? "true" : "false";
    }

    std::string operator()(List &list) {
        std::string s = "(";
        for (const auto &v : list.list) {
            s += to_string(v) + " ";
        }
        if (s.size() > 1) {
            s.pop_back();
        }
        s += ")";
        return s;
    }

    std::string operator()(Lambda &) {
        return "lambda";
    }

    std::string operator()(String &s) {
        return "\"" + s.value + "\"";
    }
};

std::string to_string(Result res) {
    return std::visit(PrintVisitor{}, res);
}

std::set<std::string> builtins = {
        "+", "-", "/", "*", "let", "println", "do", "define", "lambda", "=", "list", "first", "rest", "cond", "if", "<",
        ">", "<=", ">=", "cons", "length"
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

    } else if (op == "cond") {
        for (int i = 1; i < list.size(); ++i) {
            if (!list[i].is_list()) {
                throw std::runtime_error("Expected list of condition and expression");
            }

            auto exprs = list[i].get_list();
            if (exprs.size() < 2) {
                throw std::runtime_error("No expression following test");
            }

            bool is_else = exprs[0].is_symbol() && exprs[0].get_symbol() == "else";
            if (is_else) {
                return eval(exprs[1], env);
            }

            // Keep two ifs clauses separate because otherwise it will try to evaluate "else"
            auto test_res = eval(exprs[0], env).first;
            auto is_true = std::holds_alternative<bool>(test_res) && std::get<bool>(test_res);
            if (is_true) {
                return eval(exprs[1], env);
            }
        }

        // No clause was evaluated
        return {Nil{}, env};

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
        if (env.contains(*s)) {
            return {env.at(*s), env};
        } else {
            throw std::runtime_error("Undeclared symbol " + *s);
        }
    } else if (auto s = std::get_if<String>(&expr.value)) {
        return {*s, env};

    } else if (auto l = std::get_if<Expression::List>(&expr.value)) {
        auto list = *l;
        if (list.empty()) {
            return {Nil{}, env};
        }

        // Check if first symbol is builtin
        if (auto op = std::get_if<Symbol>(&list[0].value)) {
            if (builtins.contains(*op)) {
                return eval_builtin(*op, list, env);
            }
        }

        auto first = eval(list[0], env).first;

        if (auto lambda = std::get_if<Lambda>(&first)) {
            if (lambda->arguments.size() != list.size() - 1) {
                throw std::runtime_error(
                        "Lambda accepts " + std::to_string(lambda->arguments.size()) + " arguments, but " +
                        std::to_string(list.size() - 1) + " were provided");
            }

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

std::pair<Result, Env> eval_all(std::vector<Expression> exprs, Env env) {
    Result res;
    for (int i = 0; i < exprs.size(); ++i) {
        std::tie(res, env) = eval(exprs[i], env);
    }
    return {res, env};
}

std::pair<Result, Env> eval_with_env(std::string program, Env env) {
    auto ast = parse_all(tokenize(program));
    return eval_all(ast, env);
}


Result eval_program(std::string program) {
    auto env = eval_with_env(stdlib(), Env{}).second;
    return eval_with_env(program, env).first;
}

/*
 * standard library
 */