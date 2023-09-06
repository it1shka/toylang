project(toy_lang_tests)
add_subdirectory(lib/googletest-main)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Since my project is a small monolith binary application
# For now I'm not concerned about splitting it into multiple static libraries
# So for testing I will include everything into the build
file(GLOB lexer_src  ${CMAKE_SOURCE_DIR}/lexer/*.cpp  )
file(GLOB parser_src ${CMAKE_SOURCE_DIR}/parser/*.cpp )
file(GLOB utils_src  ${CMAKE_SOURCE_DIR}/utils.cpp    )

add_executable(tests_run lexer_basic_tests.cpp ${lexer_src} ${parser_src} ${utils_src} lexer_real_tests.cpp parser_basic_tests.cpp)
target_link_libraries(tests_run gtest gtest_main)