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

TEST(BasicParserTests, SimpleProgramTest) {
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

TEST(BasicParserTests, VariableDeclarationTest) {
    const auto program = R"(
        let a;
        let b = a + 1;
        let inc = lambda(x) { return x + 1; };
    )";
    const auto expectedOutput =
R"([program]
  [let a]
  [let b]
    [op +]
      [var a]
      [number 1]
  [let inc]
    [lambda]
      [args]
        [var x]
      [body]
        [block]
          [return]
            [op +]
              [var x]
              [number 1]
)";
    checkParser(program, expectedOutput);
}

TEST(BasicParserTests, FunctionDeclarationTest) {
    const auto program = R"(
        fun printOne() { print(1); }
        fun addOne(x) { return x + 1; }
        let makeCounter = lambda(start) {
            return lambda() {
                let current = start;
                start += 1;
                return current;
            };
        };
    )";
    const auto expectedOutput =
            R"([program]
  [fun printOne]
    [parameters]
    [body]
      [block]
        [bare expression]
          [call]
            [target]
              [var print]
            [args]
              [number 1]
  [fun addOne]
    [parameters]
      [var x]
    [body]
      [block]
        [return]
          [op +]
            [var x]
            [number 1]
  [let makeCounter]
    [lambda]
      [args]
        [var start]
      [body]
        [block]
          [return]
            [lambda]
              [args]
              [body]
                [block]
                  [let current]
                    [var start]
                  [bare expression]
                    [op +=]
                      [var start]
                      [number 1]
                  [return]
                    [var current]
)";
    checkParser(program, expectedOutput);
}

TEST(BasicParserTests, ForLoopTest) {
    const auto program = R"(
        for (i from 1 to 10) {}
        for (i from 1 + 2 to 5 + 6 step 1) {}
        for (_ from 0 to 100) {}
    )";
    const auto expectedOutput =
R"([program]
  [for loop]
    [iter i]
    [start]
      [number 1]
    [end]
      [number 10]
    [body]
      [block]
  [for loop]
    [iter i]
    [start]
      [op +]
        [number 1]
        [number 2]
    [end]
      [op +]
        [number 5]
        [number 6]
    [step]
      [number 1]
    [body]
      [block]
  [for loop]
    [iter _]
    [start]
      [number 0]
    [end]
      [number 100]
    [body]
      [block]
)";
    checkParser(program, expectedOutput);
}

TEST(BasicParserTests, WhileLoopTest) {
    const auto program = R"(
        fun weirdFactorial(n) {
            if (n < 2) return 1;
            let output = 1;
            let i = 1;
            while (i <= n) {
                output *= i;
                i += 1;
            }
            return output;
        }
    )";
    const auto expectedOutput =
R"([program]
  [fun weirdFactorial]
    [parameters]
      [var n]
    [body]
      [block]
        [branch]
          [condition]
            [op <]
              [var n]
              [number 2]
          [main clause]
            [return]
              [number 1]
        [let output]
          [number 1]
        [let i]
          [number 1]
        [while loop]
          [condition]
            [op <=]
              [var i]
              [var n]
          [body]
            [block]
              [bare expression]
                [op *=]
                  [var output]
                  [var i]
              [bare expression]
                [op +=]
                  [var i]
                  [number 1]
        [return]
          [var output]
)";
    checkParser(program, expectedOutput);
}

TEST(BasicParserTests, RestFlowConstructionsTest) {
    const auto program = R"(
        let condition = get_flag();
        if (condition) print(0);
        else if (not condition) print(1);
        let i = 0;
        while (true) {
            print(i ^ 2 ^ 3 ^ (2 + 3 * 4 div 2));
            if (i > 100) break;
            else continue;
        }
    )";
    const auto expectedOutput =
R"([program]
  [let condition]
    [call]
      [target]
        [var get_flag]
      [args]
  [branch]
    [condition]
      [var condition]
    [main clause]
      [bare expression]
        [call]
          [target]
            [var print]
          [args]
            [number 0]
    [else clause]
      [branch]
        [condition]
          [op not]
            [var condition]
        [main clause]
          [bare expression]
            [call]
              [target]
                [var print]
              [args]
                [number 1]
  [let i]
    [number 0]
  [while loop]
    [condition]
      [bool true]
    [body]
      [block]
        [bare expression]
          [call]
            [target]
              [var print]
            [args]
              [op ^]
                [var i]
                [op ^]
                  [number 2]
                  [op ^]
                    [number 3]
                    [op +]
                      [number 2]
                      [op div]
                        [op *]
                          [number 3]
                          [number 4]
                        [number 2]
        [branch]
          [condition]
            [op >]
              [var i]
              [number 100]
          [main clause]
            [break]
          [else clause]
            [continue]
)";
    checkParser(program, expectedOutput);
}