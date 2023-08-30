#pragma once
#include <optional>
#include "ibuffer.h"
#include "token.h"

namespace lexer {
    class Lexer {
        InputBuffer buffer;
        std::optional<Token> currentToken;
        std::tuple<unsigned, unsigned> position;
        // private parsing functions
        Token readToken();
        void skipWhile(std::function<bool(char)> &predicate);
        [[nodiscard]] std::string readWhile(std::function<bool(char)> &predicate);
    public:
        explicit Lexer(std::istream &source);
        const Token& peek();
        Token next();
    };
}