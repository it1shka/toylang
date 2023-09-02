#include "parser.h"
#include "ast.h"
#include "except.h"

using namespace lexer;
using enum TokenType;
using namespace parser;
using namespace parser::AST;
using namespace parser::exceptions;

using ExpressionPtr = std::unique_ptr<Expression>;
using StatementPtr = std::unique_ptr<Statement>;

#define CATCHING_BLOCK                                                   \
    const auto startPosition = lexer.peek().position;                    \
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
    lexer(input), errors({}) {}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

Program Parser::buildAST() {
    auto output = Program();
    while (!lexer.eof()) {
        auto statement = readStatement();
        output.push_back(std::move(statement));
    }
    return output;
}

// statements

StatementPtr Parser::readStatement() {
    const auto currentValue = lexer.peek().value;
    if (currentValue == "let"     ) return readVariableDeclaration();
    if (currentValue == "fun"     ) return readFunctionDeclaration();
    if (currentValue == "for"     ) return readForLoop();
    if (currentValue == "while"   ) return readWhileLoop();
    if (currentValue == "continue") return readContinueOperator();
    if (currentValue == "break"   ) return readBreakOperator();
    if (currentValue == "return"  ) return readReturnOperator();
    if (currentValue == "{"       ) return readBlockOfStatements();
    return readBareExpression();
}

StatementPtr Parser::readVariableDeclaration() noexcept {
    CATCHING_BLOCK
        expectValueToBe("let");
        const auto identifier = expectTypeToBe(Identifier);
        std::optional<std::unique_ptr<Expression>> init(std::nullopt);
        if (nextIfValue("=")) {
            init = readExpression();
        }
        expectValueToBe(";");
        const auto declaration = new VariableDeclarationStatement(identifier, std::move(init), startPosition);
        return std::unique_ptr<VariableDeclarationStatement>(declaration);
    END_CATCHING_BLOCK(IllegalStatement, "variable declaration")
}

StatementPtr Parser::readFunctionDeclaration() noexcept {
    CATCHING_BLOCK
        expectValueToBe("fun");
        const auto name = expectTypeToBe(Identifier);
        auto paramList = readFunctionArgList();
        auto body = readStatement();
        const auto declaration = new FunctionDeclarationStatement(name, std::move(paramList), std::move(body), startPosition);
        return std::unique_ptr<FunctionDeclarationStatement>(declaration);
    END_CATCHING_BLOCK(IllegalStatement, "function declaration")
}

std::vector<ExpressionPtr> Parser::readFunctionArgList() {
    auto list = std::vector<ExpressionPtr>();
    expectValueToBe("(");
    while (!lexer.eof() && !peekValueIs(")")) {
        auto expression = readExpression();
        list.push_back(std::move(expression));
        if (!nextIfValue(",")) break;
    }
    expectValueToBe(")");
    return list;
}

StatementPtr Parser::readForLoop() noexcept {
    CATCHING_BLOCK
        expectValueToBe("for");
        expectValueToBe("(");
        const auto variable = expectTypeToBe(Identifier);

        expectValueToBe("from");
        const auto start = std::stold(expectTypeToBe(Number));
        expectValueToBe("to");
        const auto end = std::stold(expectTypeToBe(Number));

        long double step = 1;
        if (nextIfValue("step")) {
            step = std::stold(expectTypeToBe(Number));
            if (step == 0) throw ForLoopZeroStepException();
            if (start < end && step < 0) throw ForLoopIncompatibleStepException(true);
            if (start > end && step > 0) throw ForLoopIncompatibleStepException(false);
        }
        expectValueToBe(")");

        const auto parameters = std::tuple{start, end, step};
        auto body = readStatement();

        const auto loop = new ForLoopStatement(variable, parameters, std::move(body), startPosition);
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

        const auto loop = new WhileLoopStatement(std::move(condition), std::move(body), startPosition);
        return std::unique_ptr<WhileLoopStatement>(loop);
    END_CATCHING_BLOCK(IllegalStatement, "while loop")
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
        std::optional<std::unique_ptr<Expression>> expression = std::nullopt;
        if (!peekValueIs(";")) {
            expression = readExpression();
        }
        expectValueToBe(";");

        const auto ret = new ReturnOperatorStatement(std::move(expression), startPosition);
        return std::unique_ptr<ReturnOperatorStatement>(ret);
    END_CATCHING_BLOCK(IllegalStatement, "return operator")
}

StatementPtr Parser::readBareExpression() noexcept {
    CATCHING_BLOCK
        auto expression = readExpression();
        const auto bare = new ExpressionStatement(std::move(expression), startPosition);
        return std::unique_ptr<ExpressionStatement>(bare);
    END_CATCHING_BLOCK(IllegalStatement, "bare expression")
}

StatementPtr Parser::readBlockOfStatements() noexcept {
    CATCHING_BLOCK
        auto statements = std::vector<std::unique_ptr<Statement>>();
        expectValueToBe("{");
        while (!lexer.eof() && !peekValueIs("}")) {
            auto statement = readStatement();
            statements.push_back(std::move(statement));
        }
        expectValueToBe("}");
        const auto block = new BlockStatement(std::move(statements), startPosition);
        return std::unique_ptr<BlockStatement>(block);
    END_CATCHING_BLOCK(IllegalStatement, "block statement")
}

// expressions

ExpressionPtr Parser::readExpression() {
    // TODO: Implement parsing expressions
}

// TODO: implement all expression parsers

// helper functions

bool Parser::peekValueIs(const std::string &value) {
    const auto peek = lexer.peek();
    return peek.value == value;
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
    throw WrongTokenValueException(expectedValue, lexer.peek().value);
}

std::string Parser::expectTypeToBe(TokenType expectedType) {
    if (lexer.peek().type == expectedType) {
        return lexer.next().value;
    }
    throw WrongTokenTypeException(lexer.peek().type, expectedType);
}

// skips everything until next punctuation sign
void Parser::performSkip() {
    while(!lexer.eof()) {
        const auto type = lexer.next().type;
        if (type == Punctuation) return;
    }
}
