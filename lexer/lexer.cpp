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

}

void Lexer::skipWhile(std::function<bool(char)> &predicate) {
    while(!buffer.eof() && predicate(buffer.peek())) {
        buffer.next();
    }
}

std::string Lexer::readWhile(std::function<bool(char)> &predicate) {
    std::string output;
    while (!buffer.eof() && predicate(buffer.peek())) {
        const auto letter = buffer.next();
        output += letter;
    }
    return output;
}