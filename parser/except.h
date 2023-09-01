#pragma once
#include <stdexcept>
#include <string>
#include "../lexer/token.h"

namespace parser::exceptions {
    class ParserException : public std::exception {};

    class WrongTokenValueException final : public ParserException {
        const std::string message;
    public:
        WrongTokenValueException (const std::string &expected, const std::string &actual)
                : message("Expected token value '" + expected + "', '" + actual + "' was provided") {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };

    using namespace lexer;
    class WrongTokenTypeException final : public ParserException {
        const std::string message;
        [[nodiscard]] static std::string createMessage(TokenType expected, TokenType actual) {
            const auto expectedType = tokenTypeToString(expected);
            const auto actualType = tokenTypeToString(actual);
            return "Expected token type '" + expectedType +
                "', '" + actualType + " was provided";
        }
    public:
        WrongTokenTypeException(TokenType expected, TokenType actual)
            : message(createMessage(expected, actual)) {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };
}
