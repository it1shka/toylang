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
        void enterScope();
        void leaveScope();
        void executeStatement(const StatementPtr &statement);
        // TODO: Implement all these functions
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
        SharedValue executeExpression(const ExpressionPtr &expression);
    public:
        explicit Interpreter(const Storage &initialStorage = {});
        void executeProgram(Program &program);
    };
}
