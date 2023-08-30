#include <vector>
#include <iostream>
#include "lexer/token.h"

int main() {
    using namespace lexer;
    using enum TokenType;
    const auto tokens = std::vector<Token> {
        { Keyword, "let" },
        { Identifier, "a" },
        { Operator, "=" },
        { Number, "5" },
        { Punctuation, ";" },
        { EndOfFile }
    };
    for (const auto &token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}
