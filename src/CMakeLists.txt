add_library(liblisp lisp.cpp tokenizer.cpp parser.cpp types.cpp)

add_executable(cpplisp repl.cpp)
target_link_libraries(cpplisp liblisp)