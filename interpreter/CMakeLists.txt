project(toy_lang_interpreter)
include_directories(include/interpreter)
add_library(toy_lang_interpreter STATIC source/interpreter.cpp include/interpreter/interpreter.h include/interpreter/types.h source/types.cpp include/interpreter/scope.h source/scope.cpp include/interpreter/except.h include/interpreter/prelude.h source/prelude.cpp)
target_link_libraries(toy_lang_interpreter PRIVATE toy_lang_parser toy_lang_lexer toy_lang_utils)
target_include_directories(toy_lang_interpreter PUBLIC include)