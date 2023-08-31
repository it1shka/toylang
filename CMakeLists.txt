cmake_minimum_required(VERSION 3.23)
project(toy_lang)

set(CMAKE_CXX_STANDARD 23)

add_executable(toy_lang main.cpp lexer/token.cpp lexer/token.h lexer/ibuffer.cpp lexer/ibuffer.h lexer/lexer.cpp lexer/lexer.h parser/ast.h)
