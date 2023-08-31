#pragma once
#include <optional>
#include "ibuffer.h"
#include "token.h"

namespace lexer {
    class Lexer {
        InputBuffer buffer;
        std::optional<Token> currentToken;
        std::tuple<unsigned, unsigned> currentPosition;
        // private parsing functions
        Token readToken();
        [[nodiscard]] Token makeToken(TokenType type, std::string value) const;
        // read different types of tokens
        Token readIllegalToken();
        Token readWordToken();
        Token readNumberToken();
        // helper functions
        void skipWhile(const std::function<bool(char)> &predicate);
        void skipWhitespace();
        void skipComment();
        [[nodiscard]] std::string readWhile(const std::function<bool(char)> &predicate);
    public:
        explicit Lexer(std::istream &source);
        const Token& peek();
        Token next();
        bool eof();
    };
}