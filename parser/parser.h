#pragma once
#include <vector>
#include "ast.h"
#include "../lexer/lexer.h"

using namespace AST;

namespace parser {
    class Parser final {
        lexer::Lexer lexer;
        std::string skipSymbol;
    public:
        explicit Parser(std::istream &source);
        [[nodiscard]] Program buildAST();
    };
}
