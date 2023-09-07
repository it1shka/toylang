#pragma once
#include <vector>
#include <set>
#include <string>
#include "ast.h"
#include "lexer/lexer.h"

using namespace lexer;
using namespace parser::AST;
using enum TokenType;

using ExpressionParser = std::function<ExpressionPtr()>;

namespace parser {
    class Parser final {
        lexer::Lexer lexer;
        std::vector<std::string> errors;
        const ExpressionParser expressionParser;
        // statements
        StatementPtr readVariableDeclaration() noexcept;
        StatementPtr readFunctionDeclaration() noexcept;
        std::vector<ExpressionPtr> readFunctionArgList();
        StatementPtr readForLoop() noexcept;
        StatementPtr readWhileLoop() noexcept;
        StatementPtr readIfElseStatement() noexcept;
        StatementPtr readContinueOperator() noexcept;
        StatementPtr readBreakOperator() noexcept;
        StatementPtr readReturnOperator() noexcept;
        StatementPtr readBareExpression() noexcept;
        StatementPtr readBlockOfStatements() noexcept;
        // expressions
        ExpressionPtr readExpression() noexcept;
        ExpressionPtr readLeftBinOp(const std::set<std::string> &ops, const ExpressionParser &next);
        ExpressionPtr readRightBinOp(const std::set<std::string> &ops, const ExpressionParser &next);
        ExpressionPtr readPrefixOperation();
        ExpressionPtr readPostfixOperation();
        ExpressionPtr readAtomicExpression() noexcept;
        ExpressionPtr readLambdaExpression() noexcept;
        // helper functions
        bool peekValueIs(const std::string &value);
        bool peekTypeIs(TokenType type);
        bool nextIfValue(const std::string &value);
        void expectValueToBe(const std::string &expectedValue);
        std::string expectTypeToBe(TokenType expectedType);
        void performSkip();
        ExpressionParser compileParser();
    public:
        // public interface
        explicit Parser(std::istream &source);
        [[nodiscard]] const std::vector<std::string>& getErrors() const;
        StatementPtr readStatement();
        ProgramPtr readProgram();
    };
}