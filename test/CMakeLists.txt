CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(c100_test)

set(CMAKE_CXX_STANDARD 17)

include_directories(. ../src)

add_executable(${PROJECT_NAME} main.cpp test.cpp
        ../src/Lexer.cpp
        ../src/Parser.cpp
        ../src/AstNode.cpp
        ../src/PrintVisitor.cpp
        ../src/Diag.cpp)