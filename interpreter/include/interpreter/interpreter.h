#pragma once
#include "parser/ast.h"
#include "scope.h"
#include <map>

using namespace parser::AST;

namespace interpreter {
    class Interpreter final {
        enum class FlowFlag {
            SequentialFlow,
            BreakLoop,
            ContinueLoop,
            ReturnValue
        };
        static std::string flowFlagToString(FlowFlag flag);
        SharedScope scope;
        FlowFlag flowFlag;
        std::optional<SharedValue> returnValue;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        void enterScope();
        void leaveScope();
        // Statements:
        void executeStatement(const StatementPtr &statement);
        void executeVariableDeclaration(const VariableDeclarationStatement* declaration);
        void executeFunctionDeclaration(const FunctionDeclarationStatement* function);
        void executeForLoop(const ForLoopStatement* forLoop);
        void executeWhileLoop(const WhileLoopStatement* whileLoop);
        void executeIfElse(const IfElseStatement* ifElse);
        void executeContinue();
        void executeBreak();
        void executeReturn(const ReturnOperatorStatement* returnOp);
        void executeBlock(const BlockStatement* block);
        void executeBareExpression(const ExpressionStatement* bare);
        // Expressions:
        SharedValue executeExpression(const ExpressionPtr &expression);
        SharedValue executeBinaryOperationExpression(const BinaryOperationExpression* expression);
        SharedValue executeRawAssignment(const ExpressionPtr &left, const ExpressionPtr &right);
        SharedValue executePrefixOperationExpression(const PrefixOperationExpression* expression);
        SharedValue executeCallExpression(const CallExpression* expression);
        SharedValue executeIndexAccessExpression(const IndexAccessExpression* expression);
        static SharedValue executeNumberLiteralExpression(const NumberLiteralExpression* expression);
        static SharedValue executeBooleanLiteralExpression(const BooleanLiteralExpression* expression);
        static SharedValue executeStringLiteralExpression(const StringLiteralExpression* expression);
        SharedValue executeArrayLiteralExpression(const ArrayLiteralExpression* expression);
        SharedValue executeVariableExpression(const VariableExpression* expression);
        SharedValue executeLambdaExpression(const LambdaExpression* expression);
    public:
        explicit Interpreter(const Storage &initialStorage = {});
        void executeProgram(Program &program);
        const std::vector<std::string>& getErrors();
        const std::vector<std::string>& getWarnings();
    };
}
