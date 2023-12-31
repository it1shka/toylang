#pragma once
#include <stdexcept>
#include <string>
#include "lexer/token.h"

#define ENABLE_WHAT [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }

namespace parser::exceptions {
    class ParserException : public std::exception {};

    class WrongTokenValueException final : public ParserException {
        const std::string message;
    public:
        WrongTokenValueException (const std::string &expected, const Token &actualToken)
                : message("Expected token value '" + expected + "', " + actualToken.toStringShort() + " was provided") {}
        ENABLE_WHAT
    };

    using namespace lexer;
    class WrongTokenTypeException final : public ParserException {
        const std::string message;
        [[nodiscard]] static std::string createMessage(TokenType expected, const Token &actualToken) {
            const auto expectedType = tokenTypeToString(expected);
            return "Expected token type '" + expectedType +
                "', " + actualToken.toStringShort() + " was provided";
        }
    public:
        WrongTokenTypeException(TokenType expected, const Token &actualToken)
            : message(createMessage(expected, actualToken)) {}
        ENABLE_WHAT
    };

    class IllegalAtomicException final : public ParserException {
        const std::string message;
        [[nodiscard]] static std::string createMessage(const Token &token) {
            return std::string("Expected boolean, number, variable, lambda or group expression")
                + ", found: " + token.toStringShort();
        }
    public:
        explicit IllegalAtomicException(const Token &token)
            : message(createMessage(token)) {}
        ENABLE_WHAT
    };

}
