cmake_minimum_required(VERSION 3.19)
project(cpplisp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(third_party)

add_subdirectory(src)
add_subdirectory(tests)
