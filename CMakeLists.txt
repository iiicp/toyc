cmake_minimum_required(VERSION 3.10)
project(c100)
set(CMAKE_CXX_STANDARD   17)
add_executable(${PROJECT_NAME} src/main.cpp src/CodeGen.cpp src/Lexer.cpp src/Parser.cpp src/AstNode.cpp src/PrintVisitor.cpp)