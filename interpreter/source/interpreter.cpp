#include "interpreter.h"
#include "except.h"

using namespace parser::AST;
using namespace interpreter;
using namespace interpreter::exceptions;
using namespace interpreter::types;

Interpreter::Interpreter(const Storage &initialStorage)
    : flowFlag(FlowFlag::SequentialFlow),
      returnValue(std::nullopt),
      warnings({}),
      errors({}) {
    scope = LexicalScope::create();
    for (const auto &[key, value] : initialStorage) {
        scope->initVariable(key, value);
    }
}

std::string Interpreter::flowFlagToString(FlowFlag flag) {
    using enum FlowFlag;
    switch (flag) {
        case SequentialFlow: return "sequential flow";
        case BreakLoop:      return "loop break";
        case ContinueLoop:   return "loop continue";
        case ReturnValue:    return "return value";
    }
}

void Interpreter::executeProgram(Program &program) {
    for (const auto &statement : program.statements) {
        try {
            executeStatement(statement);
        } catch (const RuntimeException &exception) {
            errors.emplace_back(exception.what());
        }
        if (flowFlag != FlowFlag::SequentialFlow) {
            warnings.push_back("Warning: ignored flow operator '" + flowFlagToString(flowFlag) + "'");
            flowFlag = FlowFlag::SequentialFlow;
            returnValue = std::nullopt;
        }
    }
}

const std::vector<std::string>& Interpreter::getErrors() {
    return errors;
}

const std::vector<std::string>& Interpreter::getWarnings() {
    return warnings;
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
    #define CASTED_PTR(TYPE) static_cast<TYPE*>(statement.get())
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
                throw ErrorNodeException();
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

// STATEMENTS

void Interpreter::executeVariableDeclaration(const VariableDeclarationStatement *declaration) {
    if (declaration->value) {
        const auto value = executeExpression(*declaration->value);
        scope->initVariable(declaration->name, value);
    } else {
        scope->initVariable(declaration->name);
    }
}

void Interpreter::executeFunctionDeclaration(const FunctionDeclarationStatement *fnNode) {
    const auto fnObj =
        std::make_shared<FunctionalObject> (
            fnNode->parameters,
            fnNode->body,
            scope
        );
    scope->initVariable(fnNode->name, fnObj);
}

void Interpreter::executeForLoop(const ForLoopStatement *forLoop) {
    enterScope();

    const auto start = executeExpression(forLoop->start);
    const auto end   = executeExpression(forLoop->end);
    const auto step  = forLoop->step.has_value()
            ? executeExpression(*forLoop->step)
            : std::make_shared<NumberValue>(1);

    // without operators defined it will be hard to implement the rest

    leaveScope();
}

// EXPRESSIONS

SharedValue Interpreter::executeExpression(const ExpressionPtr &expression) {
    // TODO: ...
}