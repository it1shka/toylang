#include "parser.h"
#include "ast.h"
#include "except.h"

using namespace parser;
using namespace parser::AST;
using namespace parser::exceptions;

// public interface

Parser::Parser(std::istream &input) :
    lexer(input), skipSymbol(";"), errors({}) {}

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

// statements

#define STATEMENT_PARSER_SETUP(STOP)                  \
    skipSymbol = STOP;                                \
    const auto startPosition = lexer.peek().position; \
    const auto [line, column] = startPosition;        \
    try {

#define STATEMENT_PARSER_END(ILLEGAL)                           \
    } catch (ParserException &exception) {                      \
        performSkip();                                          \
        errors.push_back (                                      \
        std::string(exception.what()) +                         \
            " at (line " + std::to_string(line) +               \
            ", at column " + std::to_string(column) + ")"       \
        );                                                      \
        const auto illegal = ILLEGAL(startPosition);            \
        return std::make_unique<ILLEGAL>(illegal);              \
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
    STATEMENT_PARSER_SETUP(";")

    STATEMENT_PARSER_END(IllegalStatement)
}

// expressions

// helper functions

bool Parser::peekValueIs(const std::string &value) {
    const auto peek = lexer.peek();
    return peek.value == value;
}

bool Parser::peekTypeIs(TokenType type) {
    const auto peek = lexer.peek();
    return peek.type == type;
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
    while(!lexer.eof() && lexer.next().value != skipSymbol);
}
