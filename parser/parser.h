#pragma once
#include "../lexer/lexer.h"

using namespace lexer;
using enum TokenType;

namespace parser {
    class Parser final {
        lexer::Lexer lexer;
        // private parsing methods
        bool peekValueIs(const std::string &value);
        bool peekTypeIs(TokenType type);
        void expectValueToBe(const std::string &expectedValue);
        std::string expectTypeToBe(TokenType expectedType);
    public:
        explicit Parser(std::istream &source);
    };
}
