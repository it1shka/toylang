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
        StatementPtr readImportLibraryStatement() noexcept;
        StatementPtr readVariableDeclaration() noexcept;
        StatementPtr readFunctionDeclaration() noexcept;
        StatementPtr readForLoop() noexcept;
        StatementPtr readWhileLoop() noexcept;
        StatementPtr readIfElseStatement() noexcept;
        StatementPtr readContinueOperator() noexcept;
        StatementPtr readBreakOperator() noexcept;
        StatementPtr readReturnOperator() noexcept;
        StatementPtr readBareExpression() noexcept;
        StatementPtr readBlockOfStatements() noexcept;
        StatementPtr readEchoStatement() noexcept;
        // expressions
        ExpressionPtr readExpression() noexcept;
        ExpressionPtr readLeftBinOp(const std::set<std::string> &ops, const ExpressionParser &next);
        ExpressionPtr readRightBinOp(const std::set<std::string> &ops, const ExpressionParser &next);
        ExpressionPtr readPrefixOperation();
        ExpressionPtr readPostfixOperation();
        ExpressionPtr readAtomicExpression() noexcept;
        ExpressionPtr readObjectExpression() noexcept;
        ExpressionPtr readLambdaExpression() noexcept;
        // helper functions
        std::vector<ExpressionPtr> readExpressionList(const std::string &start, const std::string &end);
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
