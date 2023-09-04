// This file is dedicated primarily to printing AST
// all these functions were making header file huge
// so i put all of them right here

#include "ast.h"

using namespace parser::AST;

std::string parser::AST::programToString(const parser::AST::Program &program, unsigned tabSize) {
    auto printer = Printer(tabSize);
    for (size_t i = 0; i < program.size(); i++) {
        program[i]->acceptPrinter(printer);
        if (i != program.size() - 1) {
            printer << "\n";
        }
    }
    return printer.getAccumulate();
}

#define DEF_PRINT_FOR(CLS) void CLS::acceptPrinter(Printer &printer) const

[[nodiscard]] std::string Node::nodeLabel() const {
    const auto [line, column] = position;
    auto name = nodeName();
    name[0] = static_cast<char>(toupper(static_cast<int>(name[0])));
    return name + " at (line " + std::to_string(line) +
           ", column " + std::to_string(column) + ")";
}

DEF_PRINT_FOR(VariableDeclarationStatement) {
    printer << "let " << name;
    if (value) {
        printer << " = ";
        (*value)->acceptPrinter(printer);
    }
    printer << ";";
}

DEF_PRINT_FOR(FunctionDeclarationStatement) {
    printer << "fun " << name << " (";
    for (size_t i = 0; i < parameters.size(); i++) {
        parameters[i]->acceptPrinter(printer);
        if (i != parameters.size() - 1) {
            printer << ", ";
        }
    }
    printer << ") ";
    body->acceptPrinter(printer);
}

DEF_PRINT_FOR(ForLoopStatement) {
    printer << "for (" << variable << " from ";
    start->acceptPrinter(printer);
    printer << " to ";
    end->acceptPrinter(printer);
    if (step) {
        printer << " step ";
        (*step)->acceptPrinter(printer);
    }
    printer << ") ";
    body->acceptPrinter(printer);
}

DEF_PRINT_FOR(WhileLoopStatement) {
    printer << "while (";
    condition->acceptPrinter(printer);
    printer << ") ";
    body->acceptPrinter(printer);
}

DEF_PRINT_FOR(IfElseStatement) {
    printer << "if (";
    condition->acceptPrinter(printer);
    printer << ") ";
    mainClause->acceptPrinter(printer);
    if (elseClause) {
        printer << " else ";
        (*elseClause)->acceptPrinter(printer);
    }
}

DEF_PRINT_FOR(ContinueOperatorStatement) {
    printer << "continue;";
}

DEF_PRINT_FOR(BreakOperatorStatement) {
    printer << "break;";
}

DEF_PRINT_FOR(ReturnOperatorStatement) {
    printer << "return";
    if (expression) {
        printer << " ";
        (*expression)->acceptPrinter(printer);
    }
    printer << ";";
}

DEF_PRINT_FOR(ExpressionStatement) {
    expression->acceptPrinter(printer);
    printer << ";";
}

DEF_PRINT_FOR(BlockStatement) {
    printer << "{\n";
    printer.increaseTabLevel();
    for (const auto &each : statements) {
        printer.pad();
        each->acceptPrinter(printer);
        printer << "\n";
    }
    printer.decreaseTabLevel();
    printer.pad();
    printer << "}";
}

DEF_PRINT_FOR(IllegalStatement) {
    printer << "ERROR";
}

DEF_PRINT_FOR(BinaryOperationExpression) {
    left->acceptPrinter(printer);
    printer << " " << op << " ";
    right->acceptPrinter(printer);
}

DEF_PRINT_FOR(PrefixOperationExpression) {
    printer << op;
    expression->acceptPrinter(printer);
}

DEF_PRINT_FOR(CallExpression) {
    target->acceptPrinter(printer);
    printer << "(";
    for (size_t i = 0; i < arguments.size(); i++) {
        arguments[i]->acceptPrinter(printer);
        if (i != arguments.size() - 1) {
            printer << ", ";
        }
    }
    printer << ")";
}

DEF_PRINT_FOR(NumberLiteralExpression) {
    printer << std::to_string(value);
}

DEF_PRINT_FOR(BooleanLiteralExpression) {
    printer << (value ? "true" : "false");
}

DEF_PRINT_FOR(VariableExpression) {
    printer << name;
}

DEF_PRINT_FOR(LambdaExpression) {
    printer << "lambda(";
    for (size_t i = 0; i < parameters.size(); i++) {
        parameters[i]->acceptPrinter(printer);
        if (i != parameters.size() - 1) {
            printer << ", ";
        }
    }
    printer << ")";
    body->acceptPrinter(printer);
}

DEF_PRINT_FOR(IllegalExpression) {
    printer << "ERROR";
}