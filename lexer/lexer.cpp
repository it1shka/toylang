#include "lexer.h"

using namespace lexer;

// public methods
Lexer::Lexer(std::istream &source) : buffer(InputBuffer(source)) {
    currentToken = std::nullopt;
    position = buffer.getPosition();
}

const Token& Lexer::peek() {
    if (!currentToken) {
        const auto token = readToken();
        currentToken.emplace(token);
    }
    return *currentToken;
}

Token Lexer::next() {
    if (!currentToken) {
        return readToken();
    }
    auto token = *currentToken;
    currentToken = std::nullopt;
    return token;
}

// private methods
Token Lexer::readToken() {
    skipWhitespace();
    // TODO: ...
}

void Lexer::skipWhile(const std::function<bool(char)> &predicate) {
    while(!buffer.eof() && predicate(buffer.peek())) {
        buffer.next();
    }
}

void Lexer::skipWhitespace() {
    skipWhile([] (auto ch) {
        const auto result = isspace(static_cast<int>(ch));
        return static_cast<bool>(result);
    });
}

std::string Lexer::readWhile(const std::function<bool(char)> &predicate) {
    std::string output;
    while (!buffer.eof() && predicate(buffer.peek())) {
        const auto letter = buffer.next();
        output += letter;
    }
    return output;
}