#include "parser.h"
#include "ast.h"
#include "except.h"

using namespace parser;
using namespace parser::AST;
using namespace parser::exceptions;

Parser::Parser(std::istream &input) :
    lexer(input) {}

// private parsing methods

bool Parser::peekValueIs(const std::string &value) {
    const auto peek = lexer.peek();
    return peek.value == value;
}

bool Parser::peekTypeIs(TokenType type) {
    const auto peek = lexer.peek();
    return peek.type == type;
}

void Parser::expectValueToBe(const std::string &expectedValue) {
    const auto actualValue = lexer.peek().value;
    if (actualValue == expectedValue) {
        lexer.next();
        return;
    }
    throw WrongTokenValueException(expectedValue, actualValue);
}

std::string Parser::expectTypeToBe(TokenType expectedType) {
    if (lexer.peek().type == expectedType) {
        return lexer.next().value;
    }
    throw WrongTokenTypeException(lexer.peek().type, expectedType);
}