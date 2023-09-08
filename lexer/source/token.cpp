#include "token.h"
#include "utils/utils.h"

using namespace lexer;

std::string lexer::tokenTypeToString(TokenType type) {
    using enum TokenType;
    switch (type) {
        case Keyword:     return "keyword";
        case Identifier:  return "identifier";
        case Operator:    return "operator";
        case Punctuation: return "punctuation";
        case Number:      return "number";
        case String:      return "string";
        case EndOfFile:   return "end of file";
        case Illegal:     return "illegal";
        default:          return "unknown";
    }
}

std::string Token::toString() const {
    const auto output = toStringShort();
    const auto [line, column] = position;
    return (
            output +
            " at (line " + std::to_string(line) +
            ", column " + std::to_string(column) + ")"
    );
}

std::string Token::toStringShort() const {
    using enum TokenType;
    auto output = tokenTypeToString(type);
    if (type == String) {
        auto prepared = utils::quotedString(value, "'");
        output += " " + prepared;
    } else if (type != EndOfFile) {
        output += " '" + value + "'";
    }
    return output;
}