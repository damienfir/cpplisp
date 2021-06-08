#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <unordered_map>

#include "tokenizer.h"
#include "parser.h"

std::string stdlib();


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

    List() = default;

    explicit List(std::vector<Result> l) : list(std::move(l)) {}
};

std::string to_string(Result res);

using Env = std::unordered_map<std::string, Result>;

std::pair<Result, Env> eval(Expression expr, Env env);

std::pair<Result, Env> eval_with_env(std::string program, Env env);

Result eval_program(std::string program);
