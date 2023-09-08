cmake_minimum_required(VERSION 3.23)
project(toy_lang)
set(CMAKE_CXX_STANDARD 23)

set(DIRECTORIES "utils" "lexer" "parser" "interpreter" "app" "tests")

foreach(directory ${DIRECTORIES})
    message("Adding a subdirectory: ${directory}")
    add_subdirectory(${directory})
endforeach()
