#include "parser.h"

#include <utility>
#include <tuple>

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
