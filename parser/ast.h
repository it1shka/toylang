#pragma once
#include <tuple>
#include <utility>
#include <vector>
#include <string>
#include <optional>
#include <memory>

#define STATEMENT_TYPE(TYPE) [[nodiscard]] StatementType type() const override { return TYPE; }
#define EXPRESSION_TYPE(TYPE) [[nodiscard]] ExpressionType type() const override { return (TYPE); }

namespace AST {
    // just for convenience, I will declare
    // Position as a separate shortcut type
    using Position = std::tuple<unsigned, unsigned>;

    // Base struct for all AST constructs
    struct Node {
        const Position position;
        explicit Node(Position position)
            : position(std::move(position)) {}
    };

    // Two base structs for Statements and Expressions in language
    struct Statement : Node {
        enum class StatementType {
            VariableDeclaration,
            FunctionDeclaration,
            ForLoop,
            WhileLoop,
            ContinueOperator,
            BreakOperator,
            ReturnOperator,
            BareExpression,
        };
        explicit Statement(Position position)
            : Node(position) {}
        [[nodiscard]] virtual StatementType type() const = 0;
        virtual ~Statement() = default;
    };

    struct Expression : Node {
        enum class ExpressionType {
            BinaryOperation,
            UnaryOperation,
            NumberLiteral,
            BooleanLiteral,
            Lambda,
            Variable,
        };
        explicit Expression(Position position)
            : Node(position) {}
        [[nodiscard]] virtual ExpressionType type() const = 0;
        virtual ~Expression() = default;
    };

    // Definition for all language constructs:

    using Program = std::vector<Statement>;

    // Statements:
    using enum Statement::StatementType;

    struct VariableDeclarationStatement : Statement {
        STATEMENT_TYPE(VariableDeclaration)
        const std::string name;
        const std::optional<std::unique_ptr<Expression>> value;
        VariableDeclarationStatement (
            std::string name,
            std::optional<std::unique_ptr<Expression>> value,
            Position position
        ) : name(std::move(name)), value(std::move(value)), Statement(position) {}
    };

    struct FunctionDeclarationStatement : Statement {
        STATEMENT_TYPE(FunctionDeclaration)
        const std::string name;
        const std::vector<std::unique_ptr<Expression>> parameters;
        const std::unique_ptr<Statement> body;
        FunctionDeclarationStatement (
            std::string name,
            std::vector<std::unique_ptr<Expression>> parameters,
            std::unique_ptr<Statement> body,
            Position position
        ) : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)), Statement(position) {}
    };

    struct ForLoopStatement : Statement {
        STATEMENT_TYPE(ForLoop)
        using Number = long double;
        const std::tuple<Number, Number, Number> parameters;
        const std::unique_ptr<Statement> body;
        ForLoopStatement (
            std::tuple<Number, Number, Number> parameters,
            std::unique_ptr<Statement> body,
            Position position
        ) : parameters(std::move(parameters)), body(std::move(body)), Statement(position) {}
    };

    struct WhileLoopStatement : Statement {
        STATEMENT_TYPE(WhileLoop)
        const std::unique_ptr<Expression> condition;
        const std::unique_ptr<Statement> body;
        WhileLoopStatement (
            std::unique_ptr<Expression> condition,
            std::unique_ptr<Statement> body,
            Position position
        ) : condition(std::move(condition)), body(std::move(body)), Statement(position) {}
    };

    struct ContinueOperatorStatement : Statement {
        STATEMENT_TYPE(ContinueOperator)
        explicit ContinueOperatorStatement(Position position) : Statement(position) {}
    };

    struct BreakOperatorStatement : Statement {
        STATEMENT_TYPE(BreakOperator)
        explicit BreakOperatorStatement(Position position) : Statement(position) {}
    };

    struct ReturnOperatorStatement : Statement {
        STATEMENT_TYPE(ReturnOperator)
        const std::optional<std::unique_ptr<Expression>> expression;
        ReturnOperatorStatement (
            std::optional<std::unique_ptr<Expression>> expression,
            Position position
        ) : expression(std::move(expression)), Statement(position) {}
    };

    struct ExpressionStatement : Statement {
        STATEMENT_TYPE(BareExpression)
        const std::unique_ptr<Expression> expression;
        ExpressionStatement(
            std::unique_ptr<Expression> expression,
            Position position
        ) : expression(std::move(expression)), Statement(position) {}
    };

    // Expressions:
    using enum Expression::ExpressionType;

    struct BinaryOperationExpression : Expression {
        EXPRESSION_TYPE(BinaryOperation)
        const std::unique_ptr<Expression> left;
        const std::unique_ptr<Expression> right;
        const std::string op;
        BinaryOperationExpression (
            std::unique_ptr<Expression> left,
            std::unique_ptr<Expression> right,
            std::string op,
            Position position
        ) : left(std::move(left)), right(std::move(right)), op(std::move(op)), Expression(position) {}
    };

    struct UnaryOperationExpression : Expression {
        EXPRESSION_TYPE(UnaryOperation)
        const std::unique_ptr<Expression> expression;
        const std::string op;
        UnaryOperationExpression (
            std::unique_ptr<Expression> expression,
            std::string op,
            Position position
        ) : expression(std::move(expression)), op(std::move(op)), Expression(position) {}
    };

    struct NumberLiteralExpression : Expression {
        EXPRESSION_TYPE(NumberLiteral)
        const long double value;
        NumberLiteralExpression (
            long double value,
            Position position
        ) : value(value), Expression(position) {}
    };

    struct BooleanLiteralExpression : Expression {
        EXPRESSION_TYPE(BooleanLiteral)
        const bool value;
        BooleanLiteralExpression (
            bool value,
            Position position
        ) : value(value), Expression(position) {}
    };

    struct VariableExpression : Expression {
        EXPRESSION_TYPE(Variable)
        const std::string name;
        VariableExpression (
                std::string name,
                Position position
        ) : name(std::move(name)), Expression(position) {}
    };

    struct LambdaExpression : Expression {
        EXPRESSION_TYPE(Lambda)
        const std::vector<std::unique_ptr<Expression>> parameters;
        const std::unique_ptr<Statement> body;
        LambdaExpression (
            std::vector<std::unique_ptr<Expression>> parameters,
            std::unique_ptr<Statement> body,
            Position position
        ) : parameters(std::move(parameters)), body(std::move(body)), Expression(position) {}
    };
}