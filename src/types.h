#pragma once
#include "parser.h"

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
