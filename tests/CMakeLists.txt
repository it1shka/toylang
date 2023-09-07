project(toy_lang_tests)

# include Google Test framework for testing
add_subdirectory(lib/googletest-main)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(toy_lang_tests lexer_basic_tests.cpp lexer_real_tests.cpp parser_basic_tests.cpp)
target_link_libraries(toy_lang_tests PRIVATE gtest gtest_main toy_lang_lexer toy_lang_parser)