
add_executable(tests tests.cpp)
target_include_directories(tests PRIVATE ../third_party)
target_link_libraries(tests liblisp)

# Add Catch2 CMake modules for test discovery
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

include(CTest)
include(Catch)
catch_discover_tests(tests)