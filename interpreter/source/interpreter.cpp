#include "interpreter.h"
#include "except.h"

using namespace interpreter;
using namespace interpreter::exceptions;

Interpreter::Interpreter(const Storage &initialStorage) {
    scope = LexicalScope::create();
    for (const auto &[key, value] : initialStorage) {
        scope->initVariable(key, value);
    }
}

void Interpreter::executeProgram(Program &program) {
    for (const auto &statement : program.statements) {
        executeStatement(statement);
    }
}

void Interpreter::enterScope() {
    auto next = LexicalScope::createInner(scope);
    scope = next;
}

void Interpreter::leaveScope() {
    if (!scope->getParent().has_value()) {
        throw InternalException("trying to leave main scope");
    }
    scope = *scope->getParent();
}

void Interpreter::executeStatement(const StatementPtr &statement) {
    using enum Statement::StatementType;

}

void Interpreter::executeExpression(const ExpressionPtr &expression) {

}