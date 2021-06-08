add_executable(test test.cpp)
target_link_libraries(test liblisp)

add_executable(test_catch test_catch.cpp)
target_link_libraries(test_catch liblisp)

include(CTest)
include(Catch)
catch_discover_tests(test_catch)