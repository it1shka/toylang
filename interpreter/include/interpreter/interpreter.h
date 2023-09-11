#pragma once
#include "parser/ast.h"
#include "scope.h"
#include <map>

using namespace parser::AST;

namespace interpreter {
    class Interpreter final {
        SharedScope scope;
        void enterScope();
        void leaveScope();
        void executeStatement(const StatementPtr &statement);
        void executeExpression(const ExpressionPtr &expression);
    public:
        explicit Interpreter(const Storage &initialStorage = {});
        void executeProgram(Program &program);
    };
}
