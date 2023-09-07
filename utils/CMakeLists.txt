project(toy_lang_utils)
include_directories(include/utils)
add_library(toy_lang_utils STATIC source/utils.cpp)
target_include_directories(toy_lang_utils PUBLIC include)