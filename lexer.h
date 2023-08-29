#pragma once
#include <string>

enum class TokenType {
    Keyword, Identifier,
    Operator, Punctuation,
    Number,
};

std::string tokenTypeToString(TokenType tokenType);

struct Token {
    const TokenType type;
    const std::string value;
    [[nodiscard]] std::string toString() const;
};