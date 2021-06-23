add_library(liblisp lisp.cpp tokenizer.cpp parser.cpp types.cpp utility.cpp ast.cpp env.cpp builtins.cpp)

add_executable(cpplisp repl.cpp)
target_link_libraries(cpplisp liblisp)