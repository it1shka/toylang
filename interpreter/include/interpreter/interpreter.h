#pragma once
#include "parser/ast.h"

using namespace parser::AST;

namespace interpreter {
    class Interpreter final {
    public:
        void executeProgram(Program &program);
        // TODO: ...
    };
}
