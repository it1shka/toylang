#pragma once
#include <string>

namespace lexer {
    enum class TokenType {
        Keyword, Identifier,
        Operator, Punctuation,
        Number,
        EndOfFile, Illegal
    };

    std::string tokenTypeToString(TokenType type);

    struct Token final {
        const TokenType type;
        const std::string value;
        const std::tuple<unsigned, unsigned> position;
        [[nodiscard]] std::string toString() const;
        [[nodiscard]] std::string toStringShort() const;
    };
}