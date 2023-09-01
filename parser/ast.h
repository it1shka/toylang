#pragma once
#include <tuple>
#include <utility>
#include <vector>
#include <string>
#include <optional>
#include <memory>

#define STATEMENT_TYPE(TYPE)  [[nodiscard]] StatementType  statementType()  const override { return TYPE; }
#define EXPRESSION_TYPE(TYPE) [[nodiscard]] ExpressionType expressionType() const override { return TYPE; }
#define NODE_NAME(NAME)       [[nodiscard]] std::string    nodeName()       const override { return NAME; }

namespace parser::AST {
    // just for convenience, I will declare
    // Position as a separate shortcut type
    using Position = std::tuple<unsigned, unsigned>;

    // Base struct for all AST constructs
    struct Node {
        const Position position;
        explicit Node(Position position)
            : position(std::move(position)) {}
        [[nodiscard]] virtual std::string nodeName() const {
            return "node";
        }
        [[nodiscard]] std::string nodeLabel() const {
            const auto [line, column] = position;
            auto name = nodeName();
            name[0] = static_cast<char>(toupper(static_cast<int>(name[0])));
            return name + " at (line " + std::to_string(line) +
                ", column " + std::to_string(column) + ")";
        }
    };

    // Two base structs for Statements and Expressions in language
    struct Statement : Node {
        NODE_NAME("statement")
        enum class StatementType {
            VariableDeclaration,
            FunctionDeclaration,
            ForLoop,
            WhileLoop,
            ContinueOperator,
            BreakOperator,
            ReturnOperator,
            BareExpression,
            BlockOfStatements,
            StatementError
        };
        explicit Statement(Position position)
            : Node(position) {}
        [[nodiscard]] virtual StatementType statementType() const = 0;
        virtual ~Statement() = default;
    };

    struct Expression : Node {
        NODE_NAME("expression")
        enum class ExpressionType {
            BinaryOperation,
            UnaryOperation,
            NumberLiteral,
            BooleanLiteral,
            Lambda,
            Variable,
            ExpressionError
        };
        explicit Expression(Position position)
            : Node(position) {}
        [[nodiscard]] virtual ExpressionType expressionType() const = 0;
        virtual ~Expression() = default;
    };

    // Definition for all language constructs:

    // Program is a list of unique pointers to statements
    using Program = std::vector<std::unique_ptr<Statement>>;

    // Statements:
    using enum Statement::StatementType;

    struct VariableDeclarationStatement final : Statement {
        NODE_NAME("variable declaration")
        STATEMENT_TYPE(VariableDeclaration)
        const std::string name;
        const std::optional<std::unique_ptr<Expression>> value;
        VariableDeclarationStatement (
            std::string name,
            std::optional<std::unique_ptr<Expression>> value,
            Position position
        ) : name(std::move(name)), value(std::move(value)), Statement(position) {}
    };

    struct FunctionDeclarationStatement final : Statement {
        NODE_NAME("function declaration")
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

    struct ForLoopStatement final : Statement {
        NODE_NAME("for loop")
        STATEMENT_TYPE(ForLoop)
        using Number = long double;
        const std::string variable;
        const std::tuple<Number, Number, Number> parameters;
        const std::unique_ptr<Statement> body;
        ForLoopStatement (
            std::string variable,
            std::tuple<Number, Number, Number> parameters,
            std::unique_ptr<Statement> body,
            Position position
        ) : variable(std::move(variable)), parameters(std::move(parameters)), body(std::move(body)), Statement(position) {}
    };

    struct WhileLoopStatement final : Statement {
        NODE_NAME("while loop")
        STATEMENT_TYPE(WhileLoop)
        const std::unique_ptr<Expression> condition;
        const std::unique_ptr<Statement> body;
        WhileLoopStatement (
            std::unique_ptr<Expression> condition,
            std::unique_ptr<Statement> body,
            Position position
        ) : condition(std::move(condition)), body(std::move(body)), Statement(position) {}
    };

    struct ContinueOperatorStatement final : Statement {
        NODE_NAME("continue operator")
        STATEMENT_TYPE(ContinueOperator)
        explicit ContinueOperatorStatement(Position position) : Statement(position) {}
    };

    struct BreakOperatorStatement final : Statement {
        NODE_NAME("break operator")
        STATEMENT_TYPE(BreakOperator)
        explicit BreakOperatorStatement(Position position) : Statement(position) {}
    };

    struct ReturnOperatorStatement final : Statement {
        NODE_NAME("return operator")
        STATEMENT_TYPE(ReturnOperator)
        const std::optional<std::unique_ptr<Expression>> expression;
        ReturnOperatorStatement (
            std::optional<std::unique_ptr<Expression>> expression,
            Position position
        ) : expression(std::move(expression)), Statement(position) {}
    };

    struct ExpressionStatement final : Statement {
        NODE_NAME("expression statement")
        STATEMENT_TYPE(BareExpression)
        const std::unique_ptr<Expression> expression;
        ExpressionStatement (
            std::unique_ptr<Expression> expression,
            Position position
        ) : expression(std::move(expression)), Statement(position) {}
    };

    struct BlockStatement final : Statement {
        NODE_NAME("block statement")
        STATEMENT_TYPE(BlockOfStatements)
        const std::vector<std::unique_ptr<Statement>> statements;
        BlockStatement (
            std::vector<std::unique_ptr<Statement>> statements,
            Position position
        ) : statements(std::move(statements)), Statement(position) {}
    };

    struct IllegalStatement final : Statement {
        NODE_NAME("illegal statement")
        STATEMENT_TYPE(StatementError)
        explicit IllegalStatement (Position position) : Statement(position) {}
    };

    // Expressions:
    using enum Expression::ExpressionType;

    struct BinaryOperationExpression final : Expression {
        NODE_NAME("binary operation")
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

    struct UnaryOperationExpression final : Expression {
        NODE_NAME("unary operation")
        EXPRESSION_TYPE(UnaryOperation)
        const std::unique_ptr<Expression> expression;
        const std::string op;
        UnaryOperationExpression (
            std::unique_ptr<Expression> expression,
            std::string op,
            Position position
        ) : expression(std::move(expression)), op(std::move(op)), Expression(position) {}
    };

    struct NumberLiteralExpression final : Expression {
        NODE_NAME("number literal")
        EXPRESSION_TYPE(NumberLiteral)
        const long double value;
        NumberLiteralExpression (
            long double value,
            Position position
        ) : value(value), Expression(position) {}
    };

    struct BooleanLiteralExpression final : Expression {
        NODE_NAME("boolean literal")
        EXPRESSION_TYPE(BooleanLiteral)
        const bool value;
        BooleanLiteralExpression (
            bool value,
            Position position
        ) : value(value), Expression(position) {}
    };

    struct VariableExpression final : Expression {
        NODE_NAME("variable")
        EXPRESSION_TYPE(Variable)
        const std::string name;
        VariableExpression (
                std::string name,
                Position position
        ) : name(std::move(name)), Expression(position) {}
    };

    struct LambdaExpression final : Expression {
        NODE_NAME("lambda function")
        EXPRESSION_TYPE(Lambda)
        const std::vector<std::unique_ptr<Expression>> parameters;
        const std::unique_ptr<Statement> body;
        LambdaExpression (
            std::vector<std::unique_ptr<Expression>> parameters,
            std::unique_ptr<Statement> body,
            Position position
        ) : parameters(std::move(parameters)), body(std::move(body)), Expression(position) {}
    };

    struct IllegalExpression final : Expression {
        NODE_NAME("illegal expression")
        EXPRESSION_TYPE(ExpressionError)
        explicit IllegalExpression (Position position) : Expression(position) {}
    };
}