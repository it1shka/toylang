#pragma once
#include "../lexer/lexer.h"
#include <vector>
#include <string>
#include "ast.h"

using namespace lexer;
using namespace parser::AST;
using enum TokenType;

namespace parser {
    class Parser final {
        lexer::Lexer lexer;
        std::vector<std::string> errors;
        // private parsing methods
        // statements
        using StatementPtr = std::unique_ptr<Statement>;
        StatementPtr readStatement();
        StatementPtr readVariableDeclaration();
        StatementPtr readFunctionDeclaration();
        StatementPtr readForLoop();
        StatementPtr readWhileLoop();
        StatementPtr readContinueOperator();
        StatementPtr readBreakOperator();
        StatementPtr readReturnOperator();
        StatementPtr readBareExpression();
        StatementPtr readBlockOfStatements();
        // TODO: implement all these guys
        // expressions
        using ExpressionPtr = std::unique_ptr<Expression>;
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
