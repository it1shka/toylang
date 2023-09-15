#include "interpreter.h"
#include "except.h"
#include "utils/utils.h"

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

#define CATCH_PROPAGATE(NODE)                                           \
    catch (...) {                                                       \
        const auto currentExpression = std::current_exception();        \
        std::string description = "unknown runtime exception";          \
        try {                                                           \
            std::rethrow_exception(currentExpression);                  \
        } catch (const RuntimeException &exception) {                   \
            description = std::string(exception.what());                \
        }                                                               \
        throw PropagatedException(NODE->nodeLabel(), description);      \
    }

void Interpreter::executeStatement(const StatementPtr &statement) {
    #define STMT_PTR(TYPE) static_cast<TYPE*>(statement.get())
    try {
        using enum Statement::StatementType;
        switch(statement->statementType()) {
            case LibraryImport:
                throw UnimplementedException("import");
            case VariableDeclaration:
                return executeVariableDeclaration(STMT_PTR(VariableDeclarationStatement ));
            case FunctionDeclaration:
                return executeFunctionDeclaration(STMT_PTR(FunctionDeclarationStatement ));
            case ForLoop:
                return executeForLoop            (STMT_PTR(ForLoopStatement             ));
            case WhileLoop:
                return executeWhileLoop          (STMT_PTR(WhileLoopStatement           ));
            case IfElse:
                return executeIfElse             (STMT_PTR(IfElseStatement              ));
            case ContinueOperator:
                return executeContinue();
            case BreakOperator:
                return executeBreak();
            case ReturnOperator:
                return executeReturn             (STMT_PTR(ReturnOperatorStatement      ));
            case BlockOfStatements:
                return executeBlock              (STMT_PTR(BlockStatement               ));
            case BareExpression:
                return executeBareExpression     (STMT_PTR(ExpressionStatement          ));
            case StatementError:
                throw ErrorNodeException();
        }
    } CATCH_PROPAGATE(statement)
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

#define LOOP_FLOW_CHECK                         \
       using enum FlowFlag;                     \
        if (flowFlag == BreakLoop) {            \
            flowFlag = SequentialFlow;          \
            break;                              \
        } else if (flowFlag == ContinueLoop) {  \
            flowFlag = SequentialFlow;          \
        } else if (flowFlag == ReturnValue) {   \
            break;                              \
        }

void Interpreter::executeForLoop(const ForLoopStatement *forLoop) {
    enterScope();

    auto counter = executeExpression(forLoop->start);
    const auto end   = executeExpression(forLoop->end);
    const auto step  = forLoop->step.has_value()
            ? executeExpression(*forLoop->step)
            : std::make_shared<NumberValue>(1);

    scope->initVariable(forLoop->variable, counter);
    while (true) {
        if (*counter >= end) break;
        executeStatement(forLoop->body);
        LOOP_FLOW_CHECK
        (*counter) += step;
    }

    leaveScope();
}

void Interpreter::executeWhileLoop(const WhileLoopStatement *whileLoop) {
    while (true) {
        const auto conditionResult = executeExpression(whileLoop->condition);
        const auto boolResult = types::getCastedPointer<BooleanType, BooleanValue>(conditionResult);
        if (!boolResult->value) break;
        executeStatement(whileLoop->body);
        LOOP_FLOW_CHECK
    }
}

void Interpreter::executeIfElse(const IfElseStatement *ifElse) {
    const auto conditionResult = executeExpression(ifElse->condition);
    const auto boolResult = types::getCastedPointer<BooleanType, BooleanValue>(conditionResult);
    if (boolResult->value) {
        executeStatement(ifElse->mainClause);
    } else if (ifElse->elseClause.has_value()) {
        executeStatement(*ifElse->elseClause);
    }
}

void Interpreter::executeContinue() {
    flowFlag = FlowFlag::ContinueLoop;
}

void Interpreter::executeBreak() {
    flowFlag = FlowFlag::BreakLoop;
}

void Interpreter::executeReturn(const ReturnOperatorStatement *returnOp) {
    flowFlag = FlowFlag::ReturnValue;
    if (returnOp->expression.has_value()) {
        returnValue = executeExpression(*returnOp->expression);
    }
}

void Interpreter::executeBlock(const BlockStatement *block) {
    enterScope();
    for (const auto &each : block->statements) {
        executeStatement(each);
        if (flowFlag != FlowFlag::SequentialFlow) {
            break;
        }
    }
    leaveScope();
}

void Interpreter::executeBareExpression(const ExpressionStatement *bare) {
    executeExpression(bare->expression);
}

// EXPRESSIONS

SharedValue Interpreter::executeExpression(const ExpressionPtr &expression) {
    #define EXPR_PTR(TYPE) static_cast<TYPE*>(expression.get())
    try {
        using enum Expression::ExpressionType;
        switch (expression->expressionType()) {
            case BinaryOperation:
                return executeBinaryOperationExpression(EXPR_PTR(BinaryOperationExpression));
            case PrefixOperation:
                return executePrefixOperationExpression(EXPR_PTR(PrefixOperationExpression));
            case Call:
                return executeCallExpression           (EXPR_PTR(CallExpression           ));
            case IndexAccess:
                return executeIndexAccessExpression    (EXPR_PTR(IndexAccessExpression    ));
            case NumberLiteral:
                return executeNumberLiteralExpression  (EXPR_PTR(NumberLiteralExpression  ));
            case BooleanLiteral:
                return executeBooleanLiteralExpression (EXPR_PTR(BooleanLiteralExpression ));
            case StringLiteral:
                return executeStringLiteralExpression  (EXPR_PTR(StringLiteralExpression  ));
            case ArrayLiteral:
                return executeArrayLiteralExpression   (EXPR_PTR(ArrayLiteralExpression   ));
            case NilLiteral:
                return NilValue::getInstance();
            case Variable:
                return executeVariableExpression       (EXPR_PTR(VariableExpression       ));
            case Lambda:
                return executeLambdaExpression         (EXPR_PTR(LambdaExpression         ));
            case ExpressionError:
                throw ErrorNodeException               ();
        }
    } CATCH_PROPAGATE(expression)
}


SharedValue Interpreter::executeBinaryOperationExpression(const BinaryOperationExpression *expression) {
    if (expression->op == "=") return executeRawAssignment(expression->left, expression->right);

    const auto left = executeExpression(expression->left);
    const auto right = executeExpression(expression->right);
    #define DEF_BIN_OP(OP_NAME,OP_VAL) if (expression->op == OP_NAME) return *left OP_VAL right;

    DEF_BIN_OP("or",  ||)
    DEF_BIN_OP("and", &&)
    DEF_BIN_OP("==",  ==)
    DEF_BIN_OP("!=",  !=)
    DEF_BIN_OP(">",   > )
    DEF_BIN_OP("<",   < )
    DEF_BIN_OP(">=",  >=)
    DEF_BIN_OP("<=",  <=)
    DEF_BIN_OP("-",   - )
    DEF_BIN_OP("+",   + )
    DEF_BIN_OP("*",   * )
    DEF_BIN_OP("/",   / )
    DEF_BIN_OP("div", & )
    DEF_BIN_OP("mod", % )
    DEF_BIN_OP("^",   ^ )

    throw UnsupportedOperator(expression->op);
}

SharedValue Interpreter::executeRawAssignment(const ExpressionPtr &left, const ExpressionPtr &right) {
    auto rvalue = executeExpression(right);
    auto copy = copyForAssignment(rvalue);

    if (left->expressionType() == Variable) {
        const auto varExpression = static_cast<VariableExpression*>(left.get());
        scope->setValue(varExpression->name, copy);
    } else if (left->expressionType() == IndexAccess) {
        const auto indexExpression = static_cast<IndexAccessExpression*>(left.get());
        const auto maybeIndex = executeExpression(indexExpression->index);
        const auto index = getCastedPointer<NumberType, NumberValue>(maybeIndex)->value;
        if (!utils::isInteger(index)) throw NonIntegerIndex();
        const auto maybeArray = executeExpression(indexExpression->target);
        auto array = getCastedPointer<ArrayType, ArrayObject>(maybeArray)->value;
        array[static_cast<size_t>(index)] = copy;
    }

    return copy;
}

SharedValue Interpreter::executePrefixOperationExpression(const PrefixOperationExpression *expression) {
    const auto nested = executeExpression(expression->expression);
    if (expression->op == "not") {
        return !(*nested);
    }
    if (expression->op == "-") {
        return -(*nested);
    }
    throw UnsupportedOperator(expression->op);
}

SharedValue Interpreter::executeCallExpression(const CallExpression *expression) {
    enterScope();

    const auto maybeTarget = executeExpression(expression->target);
    const auto fnPtr = getCastedPointer<FunctionType, FunctionalObject>(maybeTarget);

    std::vector<std::string> unboundNames;
    for (const auto &param : fnPtr->parameters) {
        if (param->expressionType() == Variable) {
            unboundNames.push_back(static_cast<VariableExpression*>(param.get())->name);
            continue;
        }
        // TODO: ...
    }

    leaveScope();
    if (flowFlag == FlowFlag::ReturnValue) {
        flowFlag = FlowFlag::SequentialFlow;
    }
    if (returnValue.has_value()) {
        return *returnValue;
    }
    return NilValue::getInstance();
}

SharedValue Interpreter::executeIndexAccessExpression(const IndexAccessExpression *expression) {
    const auto maybeIndex = executeExpression(expression->index);
    const auto index = getCastedPointer<NumberType, NumberValue>(maybeIndex)->value;
    if (!utils::isInteger(index)) throw NonIntegerIndex();
    const auto maybeArray = executeExpression(expression->target);
    const auto array = getCastedPointer<ArrayType, ArrayObject>(maybeArray)->value;
    return array[static_cast<size_t>(index)];
}

SharedValue Interpreter::executeNumberLiteralExpression(const NumberLiteralExpression *expression) {
    return std::make_shared<NumberValue>(expression->value);
}

SharedValue Interpreter::executeBooleanLiteralExpression(const BooleanLiteralExpression *expression) {
    return std::make_shared<BooleanValue>(expression->value);
}

SharedValue Interpreter::executeStringLiteralExpression(const StringLiteralExpression *expression) {
    return std::make_shared<StringValue>(expression->value);
}

SharedValue Interpreter::executeArrayLiteralExpression(const ArrayLiteralExpression *expression) {
    std::vector<SharedValue> values;
    for (const auto &each : expression->values) {
        const auto value = executeExpression(each);
        values.push_back(value);
    }
    return std::make_shared<ArrayObject>(values);
}

SharedValue Interpreter::executeVariableExpression(const VariableExpression *expression) {
    return scope->getValue(expression->name);
}

SharedValue Interpreter::executeLambdaExpression(const LambdaExpression *expression) {
    return std::make_shared<FunctionalObject> (
        expression->parameters,
        expression->body,
        scope
    );
}
