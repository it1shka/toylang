#pragma once
#include <optional>
#include "ibuffer.h"
#include "token.h"

namespace lexer {
    class Lexer final {
        InputBuffer buffer;
        std::optional<Token> currentToken;
        std::tuple<unsigned, unsigned> currentPosition;
        // private parsing functions
        Token readToken();
        [[nodiscard]] Token makeToken(TokenType type, std::string &valueBuffer) const;
        // read different types of tokens
        Token readIllegalToken (std::string &valueBuffer);
        Token readWordToken    (std::string &valueBuffer);
        Token readNumberToken  (std::string &valueBuffer);
        Token readStringToken  (std::string &valueBuffer);
        // helper functions
        void skipWhile(const std::function<bool(char)> &predicate);
        void skipWhitespace();
        void skipComment();
        void readWhile(std::string &valueBuffer, const std::function<bool(char)> &predicate);
    public:
        explicit Lexer(std::istream &source);
        const Token& peek();
        Token next();
        bool eof();
    };
}