#pragma once
#include "../lexer/lexer.h"
#include <vector>
#include <string>
#include "ast.h"

using namespace lexer;
using namespace parser::AST;
using enum TokenType;

using StatementPtr = std::unique_ptr<Statement>;
using ExpressionPtr = std::unique_ptr<Expression>;

namespace parser {
    class Parser final {
        lexer::Lexer lexer;
        std::vector<std::string> errors;
        // statements
        StatementPtr readStatement();
        StatementPtr readVariableDeclaration() noexcept;
        StatementPtr readFunctionDeclaration() noexcept;
        std::vector<ExpressionPtr> readFunctionArgList();
        StatementPtr readForLoop() noexcept;
        StatementPtr readWhileLoop() noexcept;
        StatementPtr readContinueOperator() noexcept;
        StatementPtr readBreakOperator() noexcept;
        StatementPtr readReturnOperator() noexcept;
        StatementPtr readBareExpression() noexcept;
        StatementPtr readBlockOfStatements() noexcept;
        // expressions
        ExpressionPtr readExpression();
        // TODO: ... implement all expressions parsers
        // helper functions
        bool peekValueIs(const std::string &value);
        bool nextIfValue(const std::string &value);
        void expectValueToBe(const std::string &expectedValue);
        std::string expectTypeToBe(TokenType expectedType);
        void performSkip();
    public:
        // public interface
        explicit Parser(std::istream &source);
        [[nodiscard]] const std::vector<std::string>& getErrors() const;
        Program buildAST();
    };
}
