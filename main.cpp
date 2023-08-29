#include <iostream>
#include <vector>
#include "lexer.h"

int main() {
    using enum TokenType;
    const auto tokens = std::vector<Token> {
        { Keyword, "let" },
        { Identifier, "a" },
        { Operator, "=" },
        { Number, "5" },
        { Punctuation, ";" }
    };
    for (const auto &token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}
