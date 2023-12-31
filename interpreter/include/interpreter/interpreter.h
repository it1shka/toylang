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
        const std::string filename;
        SharedScope scope;
        FlowFlag flowRegister;
        std::optional<SharedValue> returnRegister;
        std::optional<std::string> fatalError;
        std::vector<ProgramPtr> importedASTs;
        void enterScope();
        void leaveScope();
        // Statements:
        void executeStatement(const StatementPtr &statement);
        void executeLibraryImport(const ImportLibraryStatement* import);
        void executeVariableDeclaration(const VariableDeclarationStatement* declaration);
        void executeFunctionDeclaration(const FunctionDeclarationStatement* function);
        void executeForLoop(const ForLoopStatement* forLoop);
        void executeWhileLoop(const WhileLoopStatement* whileLoop);
        void executeIfElse(const IfElseStatement* ifElse);
        void executeContinue();
        void executeBreak();
        void executeReturn(const ReturnOperatorStatement* returnOp);
        void executeBlock(const BlockStatement* block);
        void executeEcho(const EchoStatement* echo);
        void executeBareExpression(const ExpressionStatement* bare);
        SharedValue* getPlacePointer(const IndexAccessExpression* indexExpression, bool read);
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
        SharedValue executeObjectExpression(const ObjectExpression* objExpr);
    public:
        explicit Interpreter(std::string filename, const Storage& initialStorage = {});
        void executeProgram(Program &program);
        [[nodiscard]] bool didFailed() const;
        [[nodiscard]] const std::optional<std::string>& getFatalError() const;
        [[nodiscard]] std::vector<ProgramPtr>& getImportedASTs();
        [[nodiscard]] const SharedScope& getScope() const;
    };
}
