#include "parser.h"

using namespace parser;
using namespace AST;

Parser::Parser(std::istream &input) :
    lexer(input), skipSymbol(";") {}

Program Parser::buildAST() {

}