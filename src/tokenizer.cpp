#include "tokenizer.h"

#include <optional>
#include <stdexcept>

Tokens tokenize(const std::string &program) {
    Tokens tokens;
    std::optional <Token> tmp;
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
