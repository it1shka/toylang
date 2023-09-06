#include "parser.h"

#include <memory>
#include "except.h"

using namespace parser;
using namespace parser::exceptions;

#define CATCHING_BLOCK                                                   \
    auto startPosition = lexer.peek().position;                          \
    const auto [startLine, startColumn] = startPosition;                 \
    try {

#define END_CATCHING_BLOCK(TYPE,NAME)                                    \
    } catch (...) {                                                      \
        const auto currentException = std::current_exception();          \
        std::string description = "unknown error";                       \
        try {                                                            \
            std::rethrow_exception(currentException);                    \
        } catch (const ParserException &parserException) {               \
            description = std::string(parserException.what());           \
        }                                                                \
        const auto [errorLine, errorColumn] = lexer.peek().position;     \
        performSkip();                                                   \
        errors.push_back (                                               \
            "While parsing '" + std::string(NAME) +                      \
            "' (line " + std::to_string(startLine) +                     \
            + ", column " + std::to_string(startColumn) + "): " +        \
            description +                                                \
            " at (line " + std::to_string(errorLine) +                   \
            ", at column " + std::to_string(errorColumn) + ")"           \
        );                                                               \
        const auto illegal = TYPE(startPosition);                        \
        return std::make_unique<TYPE>(illegal);                          \
    }

// public interface

Parser::Parser(std::istream &input) :
    lexer(input), errors({}), expressionParser(compileParser()) {}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

ProgramPtr Parser::readProgram() {
    auto startPosition = lexer.peek().position;
    auto statements = std::vector<StatementPtr>();
    while (!lexer.eof()) {
        statements.push_back(readStatement());
    }
    const auto block = new Program(statements, startPosition);
    return std::unique_ptr<Program>(block);
}

// statements

StatementPtr Parser::readStatement() {
    const auto currentValue = lexer.peek().value;
    if (currentValue == "let"     ) return readVariableDeclaration();
    if (currentValue == "fun"     ) return readFunctionDeclaration();
    if (currentValue == "for"     ) return readForLoop();
    if (currentValue == "while"   ) return readWhileLoop();
    if (currentValue == "if"      ) return readIfElseStatement();
    if (currentValue == "continue") return readContinueOperator();
    if (currentValue == "break"   ) return readBreakOperator();
    if (currentValue == "return"  ) return readReturnOperator();
    if (currentValue == "{"       ) return readBlockOfStatements();
    return readBareExpression();
}

StatementPtr Parser::readVariableDeclaration() noexcept {
    CATCHING_BLOCK
        expectValueToBe("let");
        auto identifier = expectTypeToBe(Identifier);
        std::optional<ExpressionPtr> init(std::nullopt);
        if (nextIfValue("=")) {
            init = readExpression();
        }
        expectValueToBe(";");
        const auto declaration = new VariableDeclarationStatement(identifier, init, startPosition);
        return std::unique_ptr<VariableDeclarationStatement>(declaration);
    END_CATCHING_BLOCK(IllegalStatement, "variable declaration")
}

StatementPtr Parser::readFunctionDeclaration() noexcept {
    CATCHING_BLOCK
        expectValueToBe("fun");
        auto name = expectTypeToBe(Identifier);
        auto paramList = readFunctionArgList();
        auto body = readBlockOfStatements();
        const auto declaration = new FunctionDeclarationStatement(name, paramList, body, startPosition);
        return std::unique_ptr<FunctionDeclarationStatement>(declaration);
    END_CATCHING_BLOCK(IllegalStatement, "function declaration")
}

std::vector<ExpressionPtr> Parser::readFunctionArgList() {
    auto list = std::vector<ExpressionPtr>();
    expectValueToBe("(");
    while (!lexer.eof() && !peekValueIs(")")) {
        list.push_back(readExpression());
        if (!nextIfValue(",")) break;
    }
    expectValueToBe(")");
    return list;
}

StatementPtr Parser::readForLoop() noexcept {
    CATCHING_BLOCK
        expectValueToBe("for");
        expectValueToBe("(");
        auto variable = expectTypeToBe(Identifier);

        expectValueToBe("from");
        auto start = readExpression();
        expectValueToBe("to");
        auto end = readExpression();

        std::optional<ExpressionPtr> step = std::nullopt;
        if (nextIfValue("step")) {
            step = readExpression();
        }
        expectValueToBe(")");

        auto body = readStatement();
        const auto loop = new ForLoopStatement(variable, start, end, step, body, startPosition);
        return std::unique_ptr<ForLoopStatement>(loop);
    END_CATCHING_BLOCK(IllegalStatement, "for loop")
}

StatementPtr Parser::readWhileLoop() noexcept {
    CATCHING_BLOCK
        expectValueToBe("while");
        expectValueToBe("(");
        auto condition = readExpression();
        expectValueToBe(")");
        auto body = readStatement();
        const auto loop = new WhileLoopStatement(condition, body, startPosition);
        return std::unique_ptr<WhileLoopStatement>(loop);
    END_CATCHING_BLOCK(IllegalStatement, "while loop")
}

StatementPtr Parser::readIfElseStatement() noexcept {
    CATCHING_BLOCK
        expectValueToBe("if");
        expectValueToBe("(");
        auto condition = readExpression();
        expectValueToBe(")");
        auto mainClause = readStatement();
        std::optional<StatementPtr> elseClause = std::nullopt;
        if (nextIfValue("else")) {
            elseClause = readStatement();
        }
        const auto ifElse = new IfElseStatement(condition, mainClause, elseClause, startPosition);
        return std::unique_ptr<IfElseStatement>(ifElse);
    END_CATCHING_BLOCK(IllegalStatement, "if-else statement")
}

StatementPtr Parser::readContinueOperator() noexcept {
    CATCHING_BLOCK
        expectValueToBe("continue");
        expectValueToBe(";");
        const auto cont = new ContinueOperatorStatement(startPosition);
        return std::unique_ptr<ContinueOperatorStatement>(cont);
    END_CATCHING_BLOCK(IllegalStatement, "continue operator")
}

StatementPtr Parser::readBreakOperator() noexcept {
    CATCHING_BLOCK
        expectValueToBe("break");
        expectValueToBe(";");
        const auto brk = new BreakOperatorStatement(startPosition);
        return std::unique_ptr<BreakOperatorStatement>(brk);
    END_CATCHING_BLOCK(IllegalStatement, "break operator")
}

StatementPtr Parser::readReturnOperator() noexcept {
    CATCHING_BLOCK
        expectValueToBe("return");
        std::optional<ExpressionPtr> expression = std::nullopt;
        if (!peekValueIs(";")) {
            expression = readExpression();
        }
        expectValueToBe(";");

        const auto ret = new ReturnOperatorStatement(expression, startPosition);
        return std::unique_ptr<ReturnOperatorStatement>(ret);
    END_CATCHING_BLOCK(IllegalStatement, "return operator")
}

StatementPtr Parser::readBareExpression() noexcept {
    CATCHING_BLOCK
        auto expression = readExpression();
        expectValueToBe(";");
        const auto bare = new ExpressionStatement(expression, startPosition);
        return std::unique_ptr<ExpressionStatement>(bare);
    END_CATCHING_BLOCK(IllegalStatement, "bare expression")
}

StatementPtr Parser::readBlockOfStatements() noexcept {
    CATCHING_BLOCK
        auto statements = std::vector<StatementPtr>();
        expectValueToBe("{");
        while (!lexer.eof() && !peekValueIs("}")) {
            statements.push_back(readStatement());
        }
        expectValueToBe("}");
        const auto block = new BlockStatement(statements, startPosition);
        return std::unique_ptr<BlockStatement>(block);
    END_CATCHING_BLOCK(IllegalStatement, "block statement")
}

// expressions

ExpressionPtr Parser::readExpression() noexcept {
    CATCHING_BLOCK
        return expressionParser();
    END_CATCHING_BLOCK(IllegalExpression, "expression")
}

ExpressionPtr Parser::readLeftBinOp(const std::set<std::string> &ops, const ExpressionParser &parser) {
    auto startPosition = lexer.peek().position;
    auto left = parser();
    while (ops.contains(lexer.peek().value)) {
        auto op = lexer.next().value;
        auto right = parser();
        const auto binOp = new BinaryOperationExpression(left, right, op, startPosition);
        left = std::unique_ptr<BinaryOperationExpression>(binOp);
    }
    return left;
}

ExpressionPtr Parser::readRightBinOp(const std::set<std::string> &ops, const ExpressionParser &parser) {
    auto startPosition = lexer.peek().position;
    auto left = parser();
    if (ops.contains(lexer.peek().value)) {
        auto op = lexer.next().value;
        auto right = readRightBinOp(ops, parser);
        const auto binOp = new BinaryOperationExpression(left, right, op, startPosition);
        left = std::unique_ptr<BinaryOperationExpression>(binOp);
    }
    return left;
}

const std::set<std::string> PREFIX_OPERATORS {
    "not", "-"
};

ExpressionPtr Parser::readPrefixOperation() {
    auto startPosition = lexer.peek().position;
    if (PREFIX_OPERATORS.contains(lexer.peek().value)) {
        auto op = lexer.next().value;
        auto nested = readPrefixOperation();
        const auto prefOp = new PrefixOperationExpression(nested, op, startPosition);
        return std::unique_ptr<PrefixOperationExpression>(prefOp);
    }
    return readPostfixOperation();
}

ExpressionPtr Parser::readPostfixOperation() {
    auto startPosition = lexer.peek().position;
    auto expression = readAtomicExpression();
    while (peekValueIs("(")) {
        auto argList = readFunctionArgList();
        const auto call = new CallExpression(expression, argList, startPosition);
        expression = std::unique_ptr<CallExpression>(call);
    }
    return expression;
}

ExpressionPtr Parser::readAtomicExpression() noexcept {
    CATCHING_BLOCK
        if (nextIfValue("true")) {
            return std::make_unique<BooleanLiteralExpression>(true, startPosition);
        }
        if (nextIfValue("false")) {
            return std::make_unique<BooleanLiteralExpression>(false, startPosition);
        }
        if (nextIfValue("(")) {
            auto expression = readExpression();
            expectValueToBe(")");
            return expression;
        }
        if (peekValueIs("lambda")) {
            return readLambdaExpression();
        }
        if (peekTypeIs(Number)) {
            const auto value = std::stold(lexer.next().value);
            const auto num = new NumberLiteralExpression(value, startPosition);
            return std::unique_ptr<NumberLiteralExpression>(num);
        }
        if (peekTypeIs(Identifier)) {
            auto name = lexer.next().value;
            const auto var = new VariableExpression(name, startPosition);
            return std::unique_ptr<VariableExpression>(var);
        }
        throw IllegalAtomicException(lexer.peek());
    END_CATCHING_BLOCK(IllegalExpression, "atomic expression")
}

ExpressionPtr Parser::readLambdaExpression() noexcept {
    CATCHING_BLOCK
        expectValueToBe("lambda");
        auto argList = readFunctionArgList();
        auto body = readBlockOfStatements();
        const auto lm = new LambdaExpression(argList, body, startPosition);
        return std::unique_ptr<LambdaExpression>(lm);
    END_CATCHING_BLOCK(IllegalExpression, "lambda expression")
}

// helper functions

bool Parser::peekValueIs(const std::string &value) {
    const auto peek = lexer.peek();
    return peek.value == value;
}

bool Parser::peekTypeIs(TokenType type) {
    const auto peek = lexer.peek();
    return peek.type == type;
}

bool Parser::nextIfValue(const std::string &value) {
    if (peekValueIs(value))  {
        lexer.next();
        return true;
    }
    return false;
}

void Parser::expectValueToBe(const std::string &expectedValue) {
    if (lexer.peek().value == expectedValue) {
        lexer.next();
        return;
    }
    throw WrongTokenValueException(expectedValue, lexer.peek());
}

std::string Parser::expectTypeToBe(TokenType expectedType) {
    if (lexer.peek().type == expectedType) {
        return lexer.next().value;
    }
    throw WrongTokenTypeException(expectedType, lexer.peek());
}

// skips everything until next punctuation sign
void Parser::performSkip() {
    while(!lexer.eof()) {
        const auto type = lexer.next().type;
        if (type == Punctuation) return;
    }
}

// I will compile rules for binary operators in runtime
// I don't think I can do that effectively in comptime using a macro

struct ParserParameter {
    enum class Associativity { Left, Right };
    const std::set<std::string> operators;
    const Associativity associativity;
};

using enum ParserParameter::Associativity;

const std::vector<ParserParameter> PARSER_PARAMETERS {
        {{"^"},                               Right },
        {{"*", "/", "div", "mod"},            Left  },
        {{"+", "-"},                          Left  },
        {{">", "<", ">=", "<="},              Left  },
        {{"==", "!="},                        Left  },
        {{"and"},                             Left  },
        {{"or"},                              Left  },
        {{"=", "+=", "-=", "*=", "/=", "^="}, Left  },
};

ExpressionParser Parser::compileParser() {
    ExpressionParser parser = [this]() -> ExpressionPtr { return readPrefixOperation(); };
    for (const auto &param : PARSER_PARAMETERS) {
        ExpressionParser nextParser;
        const auto ops = param.operators;
        if (param.associativity == Left) {
            nextParser = [this, ops, parser]() -> ExpressionPtr {
                return readLeftBinOp(ops, parser);
            };
        } else {
            nextParser = [this, ops, parser]() -> ExpressionPtr {
                return readRightBinOp(ops, parser);
            };
        }
        parser = nextParser;
    }
    return parser;
}
