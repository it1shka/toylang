#pragma once
#include <tuple>
#include <utility>
#include <vector>
#include <string>
#include <optional>
#include <memory>
#include "printer.h"

#define NODE_TYPE(TYPE)       [[nodiscard]] NodeType       nodeType()       const override { return TYPE; }
#define STATEMENT_TYPE(TYPE)  [[nodiscard]] StatementType  statementType()  const override { return TYPE; }
#define EXPRESSION_TYPE(TYPE) [[nodiscard]] ExpressionType expressionType() const override { return TYPE; }
#define FORMAT                void acceptFormatPrinter(Printer &printer) const override;
#define DEBUG                 void acceptDebugPrinter (Printer &printer) const override;
#define ENABLE_PRINTING FORMAT DEBUG



namespace parser::AST {
    // just for convenience, I will declare
    // Position as a separate shortcut type
    using Position = std::tuple<unsigned, unsigned>;

    // Abstract base struct for all AST constructs
    struct Node {
        enum class NodeType {
            ProgramNode,
            StatementNode,
            ExpressionNode,
        };

        const Position position;
        explicit Node(Position &position)
            : position(std::move(position)) {}

        [[nodiscard]] virtual NodeType nodeType()  const = 0;

        // public printing functions
        [[nodiscard]] std::string toFormatString(unsigned tabSize = 4) const;
        [[nodiscard]] std::string toDebugString (unsigned tabSize = 4) const;
        // internals for printing purposes
        virtual void acceptFormatPrinter(Printer &printer) const = 0;
        virtual void acceptDebugPrinter (Printer &printer) const = 0;
    };

    // Abstract base struct for all statements
    struct Statement : Node {
        NODE_TYPE(NodeType::StatementNode)
        enum class StatementType {
            VariableDeclaration,
            FunctionDeclaration,
            ForLoop,
            WhileLoop,
            IfElse,
            ContinueOperator,
            BreakOperator,
            ReturnOperator,
            BareExpression,
            BlockOfStatements,
            StatementError
        };
        explicit Statement(Position &position)
            : Node(position) {}
        [[nodiscard]] virtual StatementType statementType() const = 0;
        virtual ~Statement() = default;
    };

    // Abstract base struct for all expressions
    struct Expression : Node {
        NODE_TYPE(NodeType::ExpressionNode)
        enum class ExpressionType {
            BinaryOperation,
            PrefixOperation,
            Call,
            NumberLiteral,
            BooleanLiteral,
            Lambda,
            Variable,
            ExpressionError
        };
        explicit Expression(Position &position)
            : Node(position) {}
        [[nodiscard]] virtual ExpressionType expressionType() const = 0;
        virtual ~Expression() = default;
    };

    // to keep my code concise, I define two new types:
    using StatementPtr = std::unique_ptr<Statement>;
    using ExpressionPtr = std::unique_ptr<Expression>;

    // Top level node containing statements. Printable
    struct Program final : Node {
        NODE_TYPE(NodeType::ProgramNode)
        const std::vector<StatementPtr> statements;
        Program (
            std::vector<StatementPtr> &statements,
            Position &position
        ) : statements(std::move(statements)), Node(position) {}
        ENABLE_PRINTING
    };

    using ProgramPtr = std::unique_ptr<Program>;

    // Definition for all language constructs:

    // Statements:
    using enum Statement::StatementType;

    struct VariableDeclarationStatement final : Statement {
        STATEMENT_TYPE(VariableDeclaration)
        const std::string name;
        const std::optional<ExpressionPtr> value;
        VariableDeclarationStatement (
            std::string &name,
            std::optional<ExpressionPtr> &value,
            Position &position
        ) : name(std::move(name)), value(std::move(value)), Statement(position) {}
    protected:
        ENABLE_PRINTING
    };

    struct FunctionDeclarationStatement final : Statement {
        STATEMENT_TYPE(FunctionDeclaration)
        const std::string name;
        const std::vector<ExpressionPtr> parameters;
        const StatementPtr body;
        FunctionDeclarationStatement (
            std::string &name,
            std::vector<ExpressionPtr> &parameters,
            StatementPtr &body,
            Position position
        ) : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)), Statement(position) {}
        ENABLE_PRINTING
    };

    struct ForLoopStatement final : Statement {
        STATEMENT_TYPE(ForLoop)
        const std::string variable;
        const ExpressionPtr start;
        const ExpressionPtr end;
        const std::optional<ExpressionPtr> step;
        const StatementPtr body;
        ForLoopStatement (
            std::string &variable,
            ExpressionPtr &start, ExpressionPtr &end, std::optional<ExpressionPtr> &step,
            StatementPtr &body,
            Position &position
        ) : variable(std::move(variable)), start(std::move(start)),
            end(std::move(end)),           step(std::move(step)),
            body(std::move(body)),         Statement(position) {}
        ENABLE_PRINTING
    };

    struct WhileLoopStatement final : Statement {
        STATEMENT_TYPE(WhileLoop)
        const ExpressionPtr condition;
        const StatementPtr body;
        WhileLoopStatement (
            ExpressionPtr &condition,
            StatementPtr &body,
            Position &position
        ) : condition(std::move(condition)), body(std::move(body)), Statement(position) {}
        ENABLE_PRINTING
    };

    struct IfElseStatement final : Statement {
        STATEMENT_TYPE(IfElse)
        const ExpressionPtr condition;
        const StatementPtr mainClause;
        const std::optional<StatementPtr> elseClause;
        IfElseStatement (
            ExpressionPtr &condition,
            StatementPtr &mainClause,
            std::optional<StatementPtr> &elseClause,
            Position &position
        ) : condition(std::move(condition)), mainClause(std::move(mainClause)),
            elseClause(std::move(elseClause)), Statement(position) {}
        ENABLE_PRINTING
    };

    struct ContinueOperatorStatement final : Statement {
        STATEMENT_TYPE(ContinueOperator)
        explicit ContinueOperatorStatement(Position &position) : Statement(position) {}
        ENABLE_PRINTING
    };

    struct BreakOperatorStatement final : Statement {
        STATEMENT_TYPE(BreakOperator)
        explicit BreakOperatorStatement(Position &position) : Statement(position) {}
        ENABLE_PRINTING
    };

    struct ReturnOperatorStatement final : Statement {
        STATEMENT_TYPE(ReturnOperator)
        const std::optional<ExpressionPtr> expression;
        ReturnOperatorStatement (
            std::optional<ExpressionPtr> &expression,
            Position &position
        ) : expression(std::move(expression)), Statement(position) {}
        ENABLE_PRINTING
    };

    struct ExpressionStatement final : Statement {
        STATEMENT_TYPE(BareExpression)
        const ExpressionPtr expression;
        ExpressionStatement (
            ExpressionPtr &expression,
            Position &position
        ) : expression(std::move(expression)), Statement(position) {}
        ENABLE_PRINTING
    };

    struct BlockStatement final : Statement {
        STATEMENT_TYPE(BlockOfStatements)
        const std::vector<StatementPtr> statements;
        BlockStatement (
            std::vector<StatementPtr> &statements,
            Position &position
        ) : statements(std::move(statements)), Statement(position) {}
        ENABLE_PRINTING
    };

    struct IllegalStatement final : Statement {
        STATEMENT_TYPE(StatementError)
        explicit IllegalStatement (Position &position) : Statement(position) {}
        ENABLE_PRINTING
    };

    // Expressions:
    using enum Expression::ExpressionType;

    struct BinaryOperationExpression final : Expression {
        EXPRESSION_TYPE(BinaryOperation)
        const ExpressionPtr left;
        const ExpressionPtr right;
        const std::string op;
        BinaryOperationExpression (
            ExpressionPtr &left,
            ExpressionPtr &right,
            std::string &op,
            Position &position
        ) : left(std::move(left)), right(std::move(right)), op(std::move(op)), Expression(position) {}
        ENABLE_PRINTING
    };

    struct PrefixOperationExpression final : Expression {
        EXPRESSION_TYPE(PrefixOperation)
        const ExpressionPtr expression;
        const std::string op;
        PrefixOperationExpression (
            ExpressionPtr &expression,
            std::string &op,
            Position &position
        ) : expression(std::move(expression)), op(std::move(op)), Expression(position) {}
        ENABLE_PRINTING
    };

    struct CallExpression final : Expression {
        EXPRESSION_TYPE(Call)
        const ExpressionPtr target;
        const std::vector<ExpressionPtr> arguments;
        CallExpression (
            ExpressionPtr &target,
            std::vector<ExpressionPtr> &arguments,
            Position &position
        ) : target(std::move(target)), arguments(std::move(arguments)), Expression(position) {}
        ENABLE_PRINTING
    };

    struct NumberLiteralExpression final : Expression {
        EXPRESSION_TYPE(NumberLiteral)
        const long double value;
        NumberLiteralExpression (
            long double value,
            Position &position
        ) : value(value), Expression(position) {}
        ENABLE_PRINTING
    };

    struct BooleanLiteralExpression final : Expression {
        EXPRESSION_TYPE(BooleanLiteral)
        const bool value;
        BooleanLiteralExpression (
            bool value,
            Position &position
        ) : value(value), Expression(position) {}
        ENABLE_PRINTING
    };

    struct VariableExpression final : Expression {
        EXPRESSION_TYPE(Variable)
        const std::string name;
        VariableExpression (
                std::string &name,
                Position &position
        ) : name(std::move(name)), Expression(position) {}
        ENABLE_PRINTING
    };

    struct LambdaExpression final : Expression {
        EXPRESSION_TYPE(Lambda)
        const std::vector<ExpressionPtr> parameters;
        const StatementPtr body;
        LambdaExpression (
            std::vector<ExpressionPtr> &parameters,
            StatementPtr &body,
            Position &position
        ) : parameters(std::move(parameters)), body(std::move(body)), Expression(position) {}
        ENABLE_PRINTING
    };

    struct IllegalExpression final : Expression {
        EXPRESSION_TYPE(ExpressionError)
        explicit IllegalExpression (Position &position) : Expression(position) {}
        ENABLE_PRINTING
    };
}