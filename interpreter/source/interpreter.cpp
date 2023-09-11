#include "interpreter.h"
#include "except.h"

using namespace interpreter;
using namespace interpreter::exceptions;
using namespace parser::AST;
using namespace interpreter::types;

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

#define CASTED_PTR(TYPE) static_cast<TYPE*>(statement.get())

void Interpreter::executeStatement(const StatementPtr &statement) {
    try {
        using enum Statement::StatementType;
        switch(statement->statementType()) {
            case LibraryImport:
                throw UnimplementedException("import");
            case VariableDeclaration:
                return executeVariableDeclaration(CASTED_PTR(VariableDeclarationStatement ));
            case FunctionDeclaration:
                return executeFunctionDeclaration(CASTED_PTR(FunctionDeclarationStatement ));
            case ForLoop:
                return executeForLoop            (CASTED_PTR(ForLoopStatement             ));
            case WhileLoop:
                return executeWhileLoop          (CASTED_PTR(WhileLoopStatement           ));
            case IfElse:
                return executeIfElse             (CASTED_PTR(IfElseStatement              ));
            case ContinueOperator:
                return executeContinue();
            case BreakOperator:
                return executeBreak();
            case ReturnOperator:
                return executeReturn             (CASTED_PTR(ReturnOperatorStatement      ));
            case BlockOfStatements:
                return executeBlock              (CASTED_PTR(BlockStatement               ));
            case BareExpression:
                return executeBareExpression     (CASTED_PTR(ExpressionStatement          ));
            case StatementError:
                throw CannotExecuteException();
        }
    } catch (...) {
        const auto currentExpression = std::current_exception();
        std::string description = "unknown runtime exception";
        try {
            std::rethrow_exception(currentExpression);
        } catch (const RuntimeException &exception) {
            description = std::string(exception.what());
        }
        throw PropagatedException(statement->nodeLabel(), description);
    }
}

void Interpreter::executeVariableDeclaration(const VariableDeclarationStatement *declaration) {
    if (declaration->value) {
        const auto value = executeExpression(*declaration->value);
        scope->initVariable(declaration->name, value);
    } else {
        scope->initVariable(declaration->name);
    }
}

void Interpreter::executeFunctionDeclaration(const FunctionDeclarationStatement *fnNode) {
    if (fnNode->body->statementType() != BlockOfStatements) {
        throw ImproperNodeException(fnNode->body->nodeName());
    }
    auto  bodyPtr = static_cast<BlockStatement*>(fnNode->body.get());
    const auto fnObj = std::make_shared<FunctionalObject>(fnNode->parameters, *bodyPtr, scope);
    scope->initVariable(fnNode->name, fnObj);
}

// TODO: implement loops and their operators



SharedValue Interpreter::executeExpression(const ExpressionPtr &expression) {
    // TODO: ...
}