#ifndef CPPLISP_LISP_H
#define CPPLISP_LISP_H

#include <string>
#include <variant>

using Number = float;
using Symbol = std::string;

struct Null {
};

struct Lambda {

};

using Result = std::variant<Null, Number, Lambda>;

Result eval_program(std::string program);

#endif //CPPLISP_LISP_H
