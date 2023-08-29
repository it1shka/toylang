cmake_minimum_required(VERSION 3.23)
project(toy_lang)

set(CMAKE_CXX_STANDARD 23)

add_executable(toy_lang main.cpp token.cpp token.h ibuffer.cpp ibuffer.h)
