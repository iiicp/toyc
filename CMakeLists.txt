cmake_minimum_required(VERSION 3.10)
project(ccc)
set(CMAKE_CXX_STANDARD   17)
set(CMAKE_BUILD_TYPE  Debug)
include_directories(src/ src/AST src/Lex src/Parse src/Sema src/Gen src/Basic)
file(GLOB_RECURSE SRCS Diag.cpp main.cpp
        "src/AST/*.cpp"
        "src/Lex/*.cpp"
        "src/Parse/*.cpp"
        "src/Sema/*.cpp"
        "src/Gen/*.cpp"
        "src/Basic/*.cpp")
add_executable(${PROJECT_NAME} ${SRCS})
