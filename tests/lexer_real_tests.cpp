#include "gtest/gtest.h"
#include "lexer/lexer.h"

using namespace lexer;
using enum TokenType;

// HELPERS

// used for simplified testing experience
struct TestToken {
    const TokenType type;
    const std::string value;
};

void performExactMatch(const std::string &program, const std::vector<TestToken> &expected) {
    auto tokenIndex = 0;
    auto stream = std::istringstream(program);
    auto lexer = Lexer(stream);
    while (!lexer.eof()) {
        const auto token = lexer.next();
        const auto &expectedToken = expected[tokenIndex++];
        if (expectedToken.value != token.value) {
            ADD_FAILURE() << "Expected value " << expectedToken.value << ", found " << token.value;
        }
        if (expectedToken.type != token.type) {
            ADD_FAILURE() << "Expected type " << tokenTypeToString(expectedToken.type) << ", found " << tokenTypeToString(token.type);
        }
    }
    EXPECT_EQ(expected.size(), tokenIndex) << "Token list sizes are not equal. ";
}

void performValueMatch(const std::string &program, const std::vector<std::string> &values) {
    auto tokenIndex = 0;
    auto stream = std::istringstream(program);
    auto lexer = Lexer(stream);
    while (!lexer.eof()) {
        const auto value = lexer.next().value;
        EXPECT_EQ(values[tokenIndex++], value) << "Token values are not equal. ";
    }
    EXPECT_EQ(values.size(), tokenIndex) << "Token list sizes are not equal. ";
}

#define INPUT             const auto input =
#define FULL_TOKENS       const auto tokens = std::vector<TestToken>
#define TOKEN_VALUES      const auto tokens = std::vector<std::string>
#define CHECK_EXACT_MATCH performExactMatch(input, tokens)
#define CHECK_VALUE_MATCH performValueMatch(input, tokens)

// TESTS

TEST(RealLexerTests, SimpleLambdaTest) {
    INPUT R"(
        let circleArea = lambda(r) {
            return PI * r ^ 2;
        };
    )";
    FULL_TOKENS {
        {Keyword, "let"},    {Identifier, "circleArea"},
        {Operator, "="},     {Keyword, "lambda"},
        {Punctuation, "("},  {Identifier, "r"},
        {Punctuation, ")"},  {Punctuation, "{"},
        {Keyword, "return"}, {Identifier, "PI"},
        {Operator, "*"},     {Identifier, "r"},
        {Operator, "^"},     {Number, "2"},
        {Punctuation, ";"},  {Punctuation, "}"},
        {Punctuation, ";"}
    };
    CHECK_EXACT_MATCH;
}

TEST(RealLexerTests, CollapsedOperatorsTest) {
    INPUT R"(
        ===!=!<==//divmod/div^not
        --+-*=*and*/==<=-=+==
    )";
    TOKEN_VALUES {
        "==", "=", "!=", "!",
        "<=", "=", "/", "/", "divmod",
        "/", "div", "^", "not",
        "-", "-", "+", "-",
        "*=", "*", "and", "*", "/=",
        "=", "<=", "-=", "+=", "="
    };
    CHECK_VALUE_MATCH;
}

TEST(RealLexerTests, UglyProgramTest) {
    INPUT R"(
        let a=1;let b=2;
    let _SUM=a+b;
    if(true)print(false);
    )";
    FULL_TOKENS {
        {Keyword, "let"}, {Identifier, "a"},
        {Operator, "="}, {Number, "1"}, {Punctuation, ";"},

        {Keyword, "let"}, {Identifier, "b"}, {Operator, "="},
        {Number, "2"}, {Punctuation, ";"},

        {Keyword, "let"}, {Identifier, "_SUM"},
        {Operator, "="}, {Identifier, "a"},
        {Operator, "+"}, {Identifier, "b"}, {Punctuation, ";"},

        {Keyword, "if"}, {Punctuation, "("}, {Keyword, "true"},
        {Punctuation, ")"}, {Identifier, "print"},
        {Punctuation, "("}, {Keyword, "false"}, {Punctuation, ")"},
        {Punctuation, ";"}
    };
    CHECK_EXACT_MATCH;
}

TEST(RealLexerTests, WrongProgramTest) {
    INPUT R"(
        let/a*=for(/=else if#else ifelse
        lambda@for from i to a (());;,,
        #not not^&a #asdqweqwe
    )";
    TOKEN_VALUES {
        "let", "/", "a",
        "*=", "for", "(", "/=",
        "else", "if",
        "lambda", "@for",
        "from", "i", "to", "a",
        "(", "(", ")", ")", ";", ";", ",", ",",
    };
    CHECK_VALUE_MATCH;
}

// NEW SYNTAX TESTS

TEST(LexerNewSyntaxTests, BasicStringsTest) {
    INPUT R"(
        "Bjarne Stroustrup"
        'Anders Hejlsberg'
        Louis Pitfold # not a string at all
        'This strings never ends
    )";
    FULL_TOKENS {
            {String, "Bjarne Stroustrup"},
            {String, "Anders Hejlsberg"},
            {Identifier, "Louis"}, {Identifier, "Pitfold"},
            {Illegal, "This strings never ends (unclosed string)"}
    };
    CHECK_EXACT_MATCH;
}

TEST(LexerNewSyntaxTests, StringFeaturesTest) {
    INPUT R"(
        'Unclosed 1
        "Unclosed 2
        'First\nSecond'
        "First\tSecond"
        "\"I Can Do That\" said Peter"
        '\'Yeah, sure\' said Anna'
        "\'I agree\' added John"
    )";
    FULL_TOKENS {
            {Illegal, "Unclosed 1 (unclosed string)"},
            {Illegal, "Unclosed 2 (unclosed string)"},
            {String, "First\nSecond"},
            {String, "First\tSecond"},
            {String, "\"I Can Do That\" said Peter"},
            {String, "'Yeah, sure' said Anna"},
            {String, "'I agree' added John"}
    };
    CHECK_EXACT_MATCH;
}

TEST(LexerNewSyntaxTests, NilTest) {
    INPUT R"(
        nil _nil nil123 Nil
    )";
    FULL_TOKENS {
            {Keyword, "nil"},
            {Identifier, "_nil"},
            {Identifier, "nil123"},
            {Identifier, "Nil"}
    };
    CHECK_EXACT_MATCH;
}

TEST(LexerNewSyntaxTests, GetBracketsOperatorsTest) {
    INPUT R"(
        array[0] object[1]
    )";
    TOKEN_VALUES {
        "array", "[", "0", "]",
        "object", "[", "1", "]",
    };
    CHECK_VALUE_MATCH;
}

TEST(LexerNewSyntaxTests, GetBracketsOperatorsTypeTest) {
    INPUT R"(
        []
    )";
    FULL_TOKENS {
            {Punctuation, "["},
            {Punctuation, "]"},
    };
    CHECK_EXACT_MATCH;
}

TEST(LexerNewSyntaxTests, ImportKeywordsTest) {
    INPUT R"(
        import MyLongLibrary as lib;
        import ShortLib;
    )";
    FULL_TOKENS {
            {Keyword, "import"},
            {Identifier, "MyLongLibrary"},
            {Keyword, "as"},
            {Identifier, "lib"},
            {Punctuation, ";"},
            {Keyword, "import"},
            {Identifier, "ShortLib"},
            {Punctuation, ";"}
    };
    CHECK_EXACT_MATCH;
}