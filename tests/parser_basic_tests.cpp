#include "gtest/gtest.h"
#include "../parser/parser.h"

using namespace parser;

void checkParser(const std::string &program, const std::string &expected) {
    std::istringstream stream(program);
    auto parser = Parser(stream);
    const auto ast = parser.readProgram();
    const auto result = ast->toDebugString(2);
    EXPECT_EQ(expected, result);
}

TEST(BasicParserTests, SimpleProgram) {
    const auto program = R"(
        let a = 1;
        let b = 2;
        let c = a + b;
    )";
    const auto expectedOutput =
R"([program]
  [let a]
    [number 1]
  [let b]
    [number 2]
  [let c]
    [op +]
      [var a]
      [var b]
)";
    checkParser(program, expectedOutput);
}

