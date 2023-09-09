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
    public:
        explicit Interpreter(const Storage &initialStorage);
        void executeProgram(Program &program);
    };
}
