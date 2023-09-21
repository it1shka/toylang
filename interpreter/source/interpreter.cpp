#include <iostream>
#include "interpreter.h"
#include "except.h"
#include "utils/utils.h"
#include "prelude.h"

using namespace parser::AST;
using namespace interpreter;
using namespace interpreter::exceptions;
using namespace interpreter::types;

Interpreter::Interpreter(const Storage &initialStorage)
    : flowRegister(FlowFlag::SequentialFlow),
      returnRegister(std::nullopt),
      fatalError(std::nullopt) {
    scope = LexicalScope::create();
    for (const auto &[key, value] : prelude::getPrelude()) {
        scope->initVariable(key, value);
    }
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
    try {
        for (const auto &statement : program.statements) {
            executeStatement(statement);
            if (flowRegister != FlowFlag::SequentialFlow) {
                const auto opName = flowFlagToString(flowRegister);
                throw MisplacedFlowOperator(opName);
            }
        }
    } catch (const RuntimeException &exception) {
        fatalError = exception.what();
    }
}

bool Interpreter::didFailed() const {
    return fatalError.has_value();
}

const std::optional<std::string> &Interpreter::getFatalError() const {
    return fatalError;
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
            case Echo:
                return executeEcho               (STMT_PTR(EchoStatement                ));
            case StatementError:
                throw ErrorNodeException();
        }
    } CATCH_PROPAGATE(statement)
}

// STATEMENTS

void Interpreter::executeVariableDeclaration(const VariableDeclarationStatement *declaration) {
    if (declaration->value) {
        const auto value = executeExpression(*declaration->value);
        const auto copied = types::copyForAssignment(value);
        scope->initVariable(declaration->name, copied);
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

#define LOOP_FLOW_CHECK                             \
       using enum FlowFlag;                         \
        if (flowRegister == BreakLoop) {            \
            flowRegister = SequentialFlow;          \
            break;                                  \
        } else if (flowRegister == ContinueLoop) {  \
            flowRegister = SequentialFlow;          \
        } else if (flowRegister == ReturnValue) {   \
            break;                                  \
        }

// TODO: Implement for loop to the negative side
void Interpreter::executeForLoop(const ForLoopStatement *forLoop) {
    const auto start = executeExpression(forLoop->start);
    const auto end   = executeExpression(forLoop->end);
    const auto step  = forLoop->step.has_value()
            ? executeExpression(*forLoop->step)
            : std::make_shared<NumberValue>(1);

    enterScope();
    scope->initVariable(forLoop->variable, start);

    while (true) {
        const auto counter = scope->getValue(forLoop->variable);
        const auto result = *counter >= end;
        const auto booleanResult = static_cast<BooleanValue*>(result.get());
        if (booleanResult->value) break;
        executeStatement(forLoop->body);
        LOOP_FLOW_CHECK
        auto nextCounter = *counter + step;
        scope->setValue(forLoop->variable, nextCounter);
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
    flowRegister = FlowFlag::ContinueLoop;
}

void Interpreter::executeBreak() {
    flowRegister = FlowFlag::BreakLoop;
}

void Interpreter::executeReturn(const ReturnOperatorStatement *returnOp) {
    if (returnOp->expression) {
        returnRegister = executeExpression(*returnOp->expression);
    }
    flowRegister = FlowFlag::ReturnValue;
}

void Interpreter::executeBlock(const BlockStatement *block) {
    enterScope();
    for (const auto &each : block->statements) {
        executeStatement(each);
        if (flowRegister != FlowFlag::SequentialFlow) {
            break;
        }
    }
    leaveScope();
}

void Interpreter::executeBareExpression(const ExpressionStatement *bare) {
    executeExpression(bare->expression);
}

void Interpreter::executeEcho(const EchoStatement *echo) {
    const auto toPrint = executeExpression(echo->expression);
    std::cout << toPrint->toString() << std::endl;
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
            case Object:
                return executeObjectExpression         (EXPR_PTR(ObjectExpression         ));
            case ExpressionError:
                throw ErrorNodeException               ();
        }
    } CATCH_PROPAGATE(expression)
}


SharedValue Interpreter::executeBinaryOperationExpression(const BinaryOperationExpression *expression) {
    if (expression->op == "=") {
        return executeRawAssignment(expression->left, expression->right);
    }

    auto left = executeExpression(expression->left);
    const auto right = executeExpression(expression->right);
    #define DEF_BIN_OP(OP_NAME,OP_VAL) if (expression->op == OP_NAME) return *left OP_VAL right;
    #define DEF_ASSIGN(OP_NAME,OP_VAL) if (expression->op == OP_NAME) { *left OP_VAL right; return left; }

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

    DEF_ASSIGN("+=",  +=)
    DEF_ASSIGN("-=",  -=)
    DEF_ASSIGN("*=",  *=)
    DEF_ASSIGN("/=",  /=)
    DEF_ASSIGN("^=",  ^=)

    throw UnsupportedOperatorException(expression->op);
}

SharedValue* Interpreter::getPlacePointer(const IndexAccessExpression* indexExpression) {
    const auto target = executeExpression(indexExpression->target);

    if (target->dataType() == ArrayType) {
        auto arrayObject = static_cast<ArrayObject*>(target.get());
        const auto maybeIndex = executeExpression(indexExpression->index);
        const auto floatingIndex = getCastedPointer<NumberType, NumberValue>(maybeIndex)->value;
        if (!utils::isInteger(floatingIndex)) throw NonIntegerIndexException();
        if (floatingIndex < 0) throw NegativeArrayIndexException();
        const auto integerIndex = static_cast<long>(floatingIndex);
        if (integerIndex >= arrayObject->value.size()) throw IndexOutOfBoundsException(integerIndex);
        return &arrayObject->value[integerIndex];
    }

    if (target->dataType() == ObjectType) {
        auto objectPtr = static_cast<UserObject*>(target.get());
        const auto key = executeExpression(indexExpression->index)->toString();
        const auto ptr = &objectPtr->value.at(key);
        return ptr;
    }

    throw WrongIndexAccessTargetException(target->getTypename());
}

SharedValue Interpreter::executeRawAssignment(const ExpressionPtr &left, const ExpressionPtr &right) {
    auto rvalue = executeExpression(right);
    auto copy = copyForAssignment(rvalue);

    if (left->expressionType() == Variable) {
        const auto varExpression = static_cast<VariableExpression*>(left.get());
        scope->setValue(varExpression->name, copy);
    } else if (left->expressionType() == IndexAccess) {
        const auto indexExpression = static_cast<IndexAccessExpression*>(left.get());
        const auto placePointer = getPlacePointer(indexExpression);
        *placePointer = copy;
    } else {
        throw ExpectedIdentifierException();
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
    throw UnsupportedOperatorException(expression->op);
}

// TODO: Fix default arguments
SharedValue Interpreter::executeCallExpression(const CallExpression *expression) {
    std::vector<SharedValue> arguments;
    for (const auto &each : expression->arguments) {
        const auto value = executeExpression(each);
        const auto copied = types::copyForAssignment(value);
        arguments.push_back(copied);
    }

    const auto maybeTarget = executeExpression(expression->target);
    if (maybeTarget->dataType() == BuiltinType) {
        const auto builtin = static_cast<BuiltinFunction*>(maybeTarget.get());
        return builtin->cppCode(arguments);
    }

    const auto fnPtr = getCastedPointer<FunctionType, FunctionalObject>(maybeTarget);
    const auto callingScope = scope;
    scope = fnPtr->scope;
    enterScope();

    std::vector<std::string> unboundNames;
    for (const auto &param : fnPtr->parameters) {
        if (param->expressionType() == Variable) {
            unboundNames.push_back(static_cast<VariableExpression*>(param.get())->name);
            continue;
        }
        if (param->expressionType() == BinaryOperation) {
            const auto binOp = static_cast<BinaryOperationExpression*>(param.get());
            if (binOp->op != "=") throw FunctionParameterWrongFormatException();
            if (binOp->left->expressionType() == Variable) {
                scope->initVariable(static_cast<VariableExpression*>(binOp->left.get())->name);
            }
            executeRawAssignment(binOp->left, binOp->right);
            continue;
        }
        throw FunctionParameterWrongFormatException();
    }

    if (unboundNames.size() != arguments.size()) {
        throw ParamsAndArgsDontMatchException(unboundNames.size(), expression->arguments.size());
    }

    for (size_t i = 0; i < unboundNames.size(); i++) {
        scope->initVariable(unboundNames[i], arguments[i]);
    }

    executeStatement(fnPtr->body);

    leaveScope();
    scope = callingScope;

    if (flowRegister != FlowFlag::ReturnValue && flowRegister != FlowFlag::SequentialFlow) {
        const auto opName = flowFlagToString(flowRegister);
        throw MisplacedFlowOperator(opName);
    }

    flowRegister = FlowFlag::SequentialFlow;
    if (returnRegister.has_value()) {
        auto value = *returnRegister;
        returnRegister = std::nullopt;
        return value;
    }

    return NilValue::getInstance();
}

SharedValue Interpreter::executeObjectExpression(const parser::AST::ObjectExpression *objExpr) {
    std::map<std::string, SharedValue> objValue;
    for (const auto& [keyExpr, valExpr] : objExpr->objectList) {
        const auto key = executeExpression(keyExpr)->toString();
        const auto value = executeExpression(valExpr);
        objValue[key] = value;
    }
    return std::make_shared<UserObject>(objValue);
}

SharedValue Interpreter::executeIndexAccessExpression(const IndexAccessExpression *expression) {
    const auto placePointer = getPlacePointer(expression);
    return *placePointer;
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
