cmake_minimum_required(VERSION 3.19)
project(cpplisp)

set(CMAKE_CXX_STANDARD 20)

add_library(liblisp src/lisp.cpp src/tokenizer.cpp src/parser.cpp)

add_executable(cpplisp src/repl.cpp)
target_link_libraries(cpplisp liblisp)

add_executable(test test/test.cpp)
target_link_libraries(test liblisp)
