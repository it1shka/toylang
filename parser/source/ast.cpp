// This file is dedicated primarily to printing AST
// all these functions were making header file huge,
// so I put all of them right here

#include "ast.h"
#include "utils/utils.h"

using namespace parser::AST;

#define FORMAT_FOR(CLS) void CLS::acceptFormatPrinter(Printer &printer) const
#define DEBUG_FOR(CLS)  void CLS::acceptDebugPrinter(Printer &printer) const

// implementing to string functions

std::string Node::nodeLabel() const {
    const auto [line, column] = position;
    return (
        nodeName() + " at (line " + std::to_string(line) +
            ", column " + std::to_string(column) + ")"
    );
}

std::string Node::toFormatString(unsigned tabSize) const {
    auto printer = Printer(tabSize);
    acceptFormatPrinter(printer);
    return printer.getAccumulate();
}

std::string Node::toDebugString(unsigned tabSize) const {
    auto printer = Printer(tabSize);
    acceptDebugPrinter(printer);
    return printer.getAccumulate();
}

// format printing

FORMAT_FOR(Program) {
    for (const auto &each : statements) {
        printer.pad();
        each->acceptFormatPrinter(printer);
        printer << "\n";
    }
}

FORMAT_FOR(ImportLibraryStatement) {
    printer << "import " << libName;
    if (alias) {
        printer << " as " << (*alias);
    }
    printer << ";";
}

FORMAT_FOR(IndexAccessExpression) {
    target->acceptFormatPrinter(printer);
    printer << "[";
    index->acceptFormatPrinter(printer);
    printer << "]";
}

FORMAT_FOR(StringLiteralExpression) {
    const auto printable = utils::quotedString(value, "\"");
    printer << printable;
}

FORMAT_FOR(NilLiteralExpression) {
    printer << "nil";
}

FORMAT_FOR(ArrayLiteralExpression) {
    printer << "[";
    for (size_t i = 0; i < values.size(); i++) {
        values[i]->acceptFormatPrinter(printer);
        if (i != values.size() - 1) {
            printer << ", ";
        }
    }
    printer << "]";
}

FORMAT_FOR(VariableDeclarationStatement) {
    printer << "let " << name;
    if (value) {
        printer << " = ";
        (*value)->acceptFormatPrinter(printer);
    }
    printer << ";";
}

FORMAT_FOR(FunctionDeclarationStatement) {
    printer << "fun " << name << " (";
    for (size_t i = 0; i < parameters.size(); i++) {
        parameters[i]->acceptFormatPrinter(printer);
        if (i != parameters.size() - 1) {
            printer << ", ";
        }
    }
    printer << ") ";
    body->acceptFormatPrinter(printer);
}

FORMAT_FOR(ForLoopStatement) {
    printer << "for (" << variable << " from ";
    start->acceptFormatPrinter(printer);
    printer << " to ";
    end->acceptFormatPrinter(printer);
    if (step) {
        printer << " step ";
        (*step)->acceptFormatPrinter(printer);
    }
    printer << ") ";
    body->acceptFormatPrinter(printer);
}

FORMAT_FOR(WhileLoopStatement) {
    printer << "while (";
    condition->acceptFormatPrinter(printer);
    printer << ") ";
    body->acceptFormatPrinter(printer);
}

FORMAT_FOR(IfElseStatement) {
    printer << "if (";
    condition->acceptFormatPrinter(printer);
    printer << ") ";
    mainClause->acceptFormatPrinter(printer);
    if (elseClause) {
        printer << " else ";
        (*elseClause)->acceptFormatPrinter(printer);
    }
}

FORMAT_FOR(ContinueOperatorStatement) {
    printer << "continue;";
}

FORMAT_FOR(BreakOperatorStatement) {
    printer << "break;";
}

FORMAT_FOR(ReturnOperatorStatement) {
    printer << "return";
    if (expression) {
        printer << " ";
        (*expression)->acceptFormatPrinter(printer);
    }
    printer << ";";
}

FORMAT_FOR(ExpressionStatement) {
    expression->acceptFormatPrinter(printer);
    printer << ";";
}

FORMAT_FOR(BlockStatement) {
    printer << "{\n";
    printer.increaseTabLevel();
    for (const auto &each : statements) {
        printer.pad();
        each->acceptFormatPrinter(printer);
        printer << "\n";
    }
    printer.decreaseTabLevel();
    printer.pad();
    printer << "}";
}

FORMAT_FOR(IllegalStatement) {
    printer << "ERROR";
}

FORMAT_FOR(BinaryOperationExpression) {
    left->acceptFormatPrinter(printer);
    printer << " " << op << " ";
    right->acceptFormatPrinter(printer);
}

FORMAT_FOR(PrefixOperationExpression) {
    printer << op;
    expression->acceptFormatPrinter(printer);
}

FORMAT_FOR(CallExpression) {
    target->acceptFormatPrinter(printer);
    printer << "(";
    for (size_t i = 0; i < arguments.size(); i++) {
        arguments[i]->acceptFormatPrinter(printer);
        if (i != arguments.size() - 1) {
            printer << ", ";
        }
    }
    printer << ")";
}

FORMAT_FOR(NumberLiteralExpression) {
    printer << utils::formatNumber(value);
}

FORMAT_FOR(BooleanLiteralExpression) {
    printer << (value ? "true" : "false");
}

FORMAT_FOR(VariableExpression) {
    printer << name;
}

FORMAT_FOR(LambdaExpression) {
    printer << "lambda(";
    for (size_t i = 0; i < parameters.size(); i++) {
        parameters[i]->acceptFormatPrinter(printer);
        if (i != parameters.size() - 1) {
            printer << ", ";
        }
    }
    printer << ")";
    body->acceptFormatPrinter(printer);
}

FORMAT_FOR(IllegalExpression) {
    printer << "ERROR";
}

// debug printing

#define NESTED_DEBUG(VALUE)                \
    printer.increaseTabLevel();            \
    (VALUE)->acceptDebugPrinter(printer);  \
    printer.decreaseTabLevel();

#define NESTED_DEBUG_EACH(CONTAINER)       \
    printer.increaseTabLevel();            \
    for (const auto &each : CONTAINER) {   \
        each->acceptDebugPrinter(printer); \
    }                                      \
    printer.decreaseTabLevel();            \

#define PUSH_LABEL(LABEL)     \
    printer.pad();            \
    printer << LABEL << "\n";

DEBUG_FOR(Program) {
    PUSH_LABEL("[program]")
    NESTED_DEBUG_EACH(statements)
}

DEBUG_FOR(ImportLibraryStatement) {
    const auto label = "[import " + libName + "]";
    PUSH_LABEL(label)
    if (alias) {
        printer.increaseTabLevel();
        const auto aliasLabel = "[alias " + (*alias) + "]";
        PUSH_LABEL(aliasLabel)
        printer.decreaseTabLevel();
    }
}

DEBUG_FOR(IndexAccessExpression) {
    PUSH_LABEL("[access]")
    printer.increaseTabLevel();
    PUSH_LABEL("[target]")
    NESTED_DEBUG(target)
    PUSH_LABEL("[index]")
    NESTED_DEBUG(index)
    printer.decreaseTabLevel();
}

DEBUG_FOR(StringLiteralExpression) {
    const auto printable = utils::quotedString(value, "\"");
    const auto label = "[str " + printable + "]";
    PUSH_LABEL(label)
}

DEBUG_FOR(NilLiteralExpression) {
    PUSH_LABEL("[nil]")
}

DEBUG_FOR(ArrayLiteralExpression) {
    PUSH_LABEL("[array]")
    NESTED_DEBUG_EACH(values)
}

DEBUG_FOR(VariableDeclarationStatement) {
    const auto varDeclLabel = "[let " + name + "]";
    PUSH_LABEL(varDeclLabel)
    if (value) {
        NESTED_DEBUG(*value)
    }
}

DEBUG_FOR(FunctionDeclarationStatement) {
    const auto funDeclLabel = "[fun " + name + "]";
    PUSH_LABEL(funDeclLabel)
    printer.increaseTabLevel();

    PUSH_LABEL("[parameters]")
    NESTED_DEBUG_EACH(parameters)

    PUSH_LABEL("[body]")
    NESTED_DEBUG(body)

    printer.decreaseTabLevel();
}

DEBUG_FOR(ForLoopStatement) {
    PUSH_LABEL("[for loop]")
    printer.increaseTabLevel();

    const auto loopVarLabel = "[iter " + variable + "]";
    PUSH_LABEL(loopVarLabel)

    PUSH_LABEL("[start]")
    NESTED_DEBUG(start)

    PUSH_LABEL("[end]")
    NESTED_DEBUG(end)

    if (step) {
        PUSH_LABEL("[step]")
        NESTED_DEBUG(*step)
    }

    PUSH_LABEL("[body]")
    NESTED_DEBUG(body)

    printer.decreaseTabLevel();
}

DEBUG_FOR(WhileLoopStatement) {
    PUSH_LABEL("[while loop]")
    printer.increaseTabLevel();

    PUSH_LABEL("[condition]")
    NESTED_DEBUG(condition)

    PUSH_LABEL("[body]")
    NESTED_DEBUG(body)

    printer.decreaseTabLevel();
}

DEBUG_FOR(IfElseStatement) {
    PUSH_LABEL("[branch]")
    printer.increaseTabLevel();

    PUSH_LABEL("[condition]")
    NESTED_DEBUG(condition)

    PUSH_LABEL("[main clause]")
    NESTED_DEBUG(mainClause)

    if (elseClause) {
        PUSH_LABEL("[else clause]")
        NESTED_DEBUG(*elseClause)
    }

    printer.decreaseTabLevel();
}

DEBUG_FOR(ContinueOperatorStatement) {
    PUSH_LABEL("[continue]")
}

DEBUG_FOR(BreakOperatorStatement) {
    PUSH_LABEL("[break]")
}

DEBUG_FOR(ReturnOperatorStatement) {
    PUSH_LABEL("[return]")
    if (expression) {
        NESTED_DEBUG(*expression)
    }
}

DEBUG_FOR(ExpressionStatement) {
    PUSH_LABEL("[bare expression]")
    NESTED_DEBUG(expression)
}

DEBUG_FOR(BlockStatement) {
    PUSH_LABEL("[block]")
    NESTED_DEBUG_EACH(statements)
}

DEBUG_FOR(IllegalStatement) {
    PUSH_LABEL("[STATEMENT ERROR]")
}

DEBUG_FOR(BinaryOperationExpression) {
    const auto binLabel = "[op " + op + "]";
    PUSH_LABEL(binLabel)
    NESTED_DEBUG(left)
    NESTED_DEBUG(right)
}

DEBUG_FOR(PrefixOperationExpression) {
    const auto prefLabel = "[op " + op + "]";
    PUSH_LABEL(prefLabel)
    NESTED_DEBUG(expression)
}

DEBUG_FOR(CallExpression) {
    PUSH_LABEL("[call]")
    printer.increaseTabLevel();

    PUSH_LABEL("[target]")
    NESTED_DEBUG(target)

    PUSH_LABEL("[args]")
    NESTED_DEBUG_EACH(arguments)

    printer.decreaseTabLevel();
}

DEBUG_FOR(NumberLiteralExpression) {
    const auto numLabel = "[number " + utils::formatNumber(value) + "]";
    PUSH_LABEL(numLabel)
}

DEBUG_FOR(BooleanLiteralExpression) {
    const auto boolLabel = value ? "[bool true]" : "[bool false]";
    PUSH_LABEL(boolLabel)
}

DEBUG_FOR(VariableExpression) {
    const auto varLabel = "[var " + name + "]";
    PUSH_LABEL(varLabel)
}

DEBUG_FOR(LambdaExpression) {
    PUSH_LABEL("[lambda]")
    printer.increaseTabLevel();

    PUSH_LABEL("[args]")
    NESTED_DEBUG_EACH(parameters)

    PUSH_LABEL("[body]")
    NESTED_DEBUG(body)

    printer.decreaseTabLevel();
}

DEBUG_FOR(IllegalExpression) {
    PUSH_LABEL("[EXPRESSION ERROR]")
}