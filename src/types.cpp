#include "types.h"

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
