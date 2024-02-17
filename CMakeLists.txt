cmake_minimum_required(VERSION 3.20)

project(scheme-intepreter)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

add_subdirectory(tests)
add_subdirectory(scheme)
add_subdirectory(repl)