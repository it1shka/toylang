#include "gtest/gtest.h"
#include "lexer/lexer.h"

using namespace lexer;

// HELPERS

void testProgramByValues(const std::string &program, const std::vector<std::string> &values, bool allowIllegal) {
    auto tokenIndex = 0;
    auto stream = std::istringstream(program);
    auto lexer = Lexer(stream);
    while (!lexer.eof()) {
        const auto token = lexer.next();
        const auto actualType = token.type;
        const auto actualValue = token.value;
        if (actualType == TokenType::Illegal && !allowIllegal) {
            ADD_FAILURE() << "Found illegal token: " + actualValue;
        }
        EXPECT_EQ(values[tokenIndex++], actualValue) << "Token values are different. ";
    }
    EXPECT_EQ(values.size(), tokenIndex) << "Token list sizes are not equal. ";
}

#define INPUT                  const auto input =
#define FULL_TOKENS                 const auto tokens = std::vector<std::string>
#define STRICT_VALUES_MATCH    testProgramByValues(input, tokens, false)
#define SOFT_VALUES_MATCH      testProgramByValues(input, tokens, true)

// TESTS:

TEST(BasicLexerTests, EmptyProgramTest) {
    INPUT R"(

    )";
    FULL_TOKENS {

    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, CommentsTest) {
    INPUT R"(
        # This is an empty program
        # because it contains only comments
        # даже по идее на другом языке
    )";
    FULL_TOKENS {

    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, CommentsAndValuesTest) {
    INPUT R"(
        # This is a variable equal to 5
        let var1 = 5;
        # This is a variable equal to 6
        let var2 = 6;
        # output
        show_number(var1 + var2);
    )";
    FULL_TOKENS {
        "let", "var1", "=", "5", ";",
        "let", "var2", "=", "6", ";",
        "show_number", "(", "var1", "+", "var2", ")", ";"
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, SimpleProgramTest) {
    INPUT R"(
        let a = 1;
        let b = 2;
        let c = a + b;
    )";
    FULL_TOKENS {
        "let", "a", "=", "1", ";",
        "let", "b", "=", "2", ";",
        "let", "c", "=", "a", "+", "b", ";",
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, KeywordsTest) {
    INPUT R"(
        let for from to step
        while continue break
        if else fun lambda
        return true false
    )";
    FULL_TOKENS {
        "let", "for", "from", "to", "step",
        "while", "continue", "break",
        "if", "else", "fun", "lambda",
        "return", "true", "false"
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, OperatorsTest) {
    INPUT R"(
        = or and
        == !=
        < > <= >=
        - + * /
        div mod
        ^
        not
    )";
    FULL_TOKENS {
        "=",
        "or", "and",
        "==", "!=",
        "<", ">", "<=", ">=",
        "-", "+", "*", "/",
        "div", "mod",
        "^",
        "not",
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, PunctuationTest) {
    INPUT R"(
        ( ) { } , ;
    )";
    FULL_TOKENS {
        "(", ")", "{", "}", ",", ";"
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, NumbersTest) {
    INPUT R"(
        0 1 2 1.123 1. 2. 333 10949802348
    )";
    FULL_TOKENS {
        "0", "1",
        "2", "1.123", "1.", "2.",
        "333", "10949802348"
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, IdentifiersTest) {
    INPUT R"(
        a b firstParameter secondParameter
        _privateVariable __veryPrivate
        plus2 plus3 looking_4_you
        I_am_18_years_old
    )";
    FULL_TOKENS {
        "a", "b", "firstParameter", "secondParameter",
        "_privateVariable", "__veryPrivate",
        "plus2", "plus3", "looking_4_you",
        "I_am_18_years_old"
    };
    STRICT_VALUES_MATCH;
}

TEST(BasicLexerTests, IllegalsTest) {
    INPUT R"(
        @decorator("something")
        def anything() -> None: ...
    )";
    FULL_TOKENS {
        "@decorator(\"something\")",
        "def", "anything", "(", ")", "-", ">", "None",
        ":", "..."
    };
    SOFT_VALUES_MATCH;
}

TEST(BasicLexerTests, ConstructionsTest) {
    INPUT R"(
        fun factorial(a) {
            if (a <= 1) return -1;
            let output = 1;
            for (i from 1 to a) {
                output *= i;
            }
            return output;
        }
    )";
    FULL_TOKENS {
        "fun", "factorial", "(", "a", ")", "{",
            "if", "(", "a", "<=", "1", ")", "return", "-", "1", ";",
            "let", "output", "=", "1", ";",
            "for", "(", "i", "from", "1", "to", "a", ")", "{",
                "output", "*=", "i", ";",
            "}",
            "return", "output", ";",
        "}",
    };
    STRICT_VALUES_MATCH;
}
