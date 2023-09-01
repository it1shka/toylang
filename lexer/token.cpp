#include "token.h"

using namespace lexer;

std::string lexer::tokenTypeToString(TokenType type) {
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
    auto output = tokenTypeToString(type);
    const auto [line, column] = position;
    if (type != TokenType::EndOfFile) {
        output += " '" + value + "'";
    }
    return (
            output +
            " at (line " + std::to_string(line) +
            ", column " + std::to_string(column) + ")"
    );
}