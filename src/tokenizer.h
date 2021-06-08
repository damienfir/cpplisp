#pragma once

#include <string>
#include <vector>

enum TokenType {
    LEFTPAREN,
    RIGHTPAREN,
    STRING,
    SYMBOL
};

struct Token {
    TokenType type;
    std::string val;

    Token(TokenType type, std::string value) : type(type), val(std::move(value)) {}

    explicit Token(TokenType type) : Token(type, "") {}
};

using Tokens = std::vector<Token>;

Tokens tokenize(const std::string &program);