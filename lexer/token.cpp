#include "token.h"

using namespace lexer;

std::string tokenTypeToString(TokenType type) {
    using enum TokenType;
    switch (type) {
        case Keyword:     return "keyword";
        case Identifier:  return "identifier";
        case Operator:    return "operator";
        case Punctuation: return "punctuation";
        case Number:      return "number";
        case EndOfFile:   return "end of file";
        case Illegal:     return "illegal";
        default:          return "unknown";
    }
}

std::string Token::toString() const {
    const auto strType = tokenTypeToString(type);
    const auto [line, column] = position;
    return (
            strType + " '" + value +
            "' at (line " + std::to_string(line) +
            ", column " + std::to_string(column) + ")"
    );
}