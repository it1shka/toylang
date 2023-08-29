#include "lexer.h"

std::string tokenTypeToString(TokenType tokenType) {
    using enum TokenType;
    switch(tokenType) {
        case Keyword:     return "keyword";
        case Identifier:  return "identifier";
        case Operator:    return "operator";
        case Punctuation: return "punctuation";
        case Number:      return "number";
    }
}

std::string Token::toString() const {
    const auto strType = tokenTypeToString(type);
    return strType + " '" + value + "'";
}