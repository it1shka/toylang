#include "parser.h"
#include "ast.h"
#include "except.h"
#include "../lexer/token.h"

using namespace lexer;
using enum TokenType;
using namespace parser;
using namespace parser::AST;
using namespace parser::exceptions;

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

// private parsing methods

#define PARSER_SETUP                                  \
    const auto startPosition = lexer.peek().position; \
    const auto [line, column] = startPosition;        \
    try {

// statements

#define PARSER_END_FOR_STATEMENT(NAME)                                   \
    } catch (ParserException &exception) {                               \
        performSkip();                                                   \
        errors.push_back (                                               \
            "While parsing " + std::string(NAME) + ": " +             \
            std::string(exception.what()) +                              \
            " at (line " + std::to_string(line) +                        \
            ", at column " + std::to_string(column) + ")"                \
        );                                                               \
        const auto illegal = IllegalStatement(startPosition);            \
        return std::make_unique<IllegalStatement>(illegal);              \
    }

using StatementPtr = std::unique_ptr<Statement>;

StatementPtr Parser::readStatement() {
    const auto currentValue = lexer.peek().value;
    if (currentValue == "let"     ) return readVariableDeclaration();
    if (currentValue == "fun"     ) return readFunctionDeclaration();
    if (currentValue == "for"     ) return readForLoop();
    if (currentValue == "while"   ) return readWhileLoop();
    if (currentValue == "continue") return readContinueOperator();
    if (currentValue == "break"   ) return readBreakOperator();
    if (currentValue == "return"  ) return readReturnOperator();
    return readBareExpression();
}

StatementPtr Parser::readVariableDeclaration() {
    PARSER_SETUP
        expectValueToBe("let");
        const auto identifier = expectTypeToBe(Identifier);
        std::optional<std::unique_ptr<Expression>> init(std::nullopt);
        if (nextIfValue("=")) {
            init = readExpression();
        }
        expectValueToBe(";");
        const auto declaration = new VariableDeclarationStatement(identifier, std::move(init), startPosition);
        return std::unique_ptr<VariableDeclarationStatement>(declaration);
    PARSER_END_FOR_STATEMENT("variable declaration")
}

StatementPtr Parser::readFunctionDeclaration() {
    PARSER_SETUP
    // TODO: ...
    PARSER_END_FOR_STATEMENT("function declaration")
}

StatementPtr Parser::readForLoop() {
    PARSER_SETUP
        expectValueToBe("for");
        expectValueToBe("(");
        const auto variable = expectTypeToBe(Identifier);
        expectValueToBe("from");
        const auto start = expectTypeToBe(Number);
        expectValueToBe("to");
        const auto end = expectTypeToBe(Number);
        // TODO: complete for loop parser
    PARSER_END_FOR_STATEMENT("for loop")
}

// TODO: implement all statement parsers

// expressions

using ExpressionPtr = std::unique_ptr<Expression>;

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

void Parser::performSkip() {
    while(!lexer.eof()) {
        const auto value = lexer.next().value;
        if (value == ";" || value == "}") return;
    }
}
