/*
 * File that describes datatypes
 * built-in to the language
 * - Nil               only one object
 * - Boolean           by value
 * - Number            by value
 * - String            by value
 * - Array             by reference
 * - Function          by reference
 */

#pragma once
#include <string>
#include <memory>
#include <vector>
#include "parser/ast.h"
#include "utils/utils.h"

#define TYPENAME(NAME) [[nodiscard]] std::string getTypename() const override { return NAME; }
#define STRING         [[nodiscard]] std::string toString() const override

using namespace parser::AST;

namespace interpreter::types {

    struct AbstractBaseType {
        [[nodiscard]] virtual std::string getTypename() const = 0;
        [[nodiscard]] virtual std::string toString()   const = 0;
    };

    using SharedValue = std::shared_ptr<AbstractBaseType>;

    struct NilValue final : AbstractBaseType {
        TYPENAME("nil")
        STRING { return "nil"; }
        static SharedValue getInstance();
    private:
        NilValue() = default;
    };

    struct BooleanValue final : AbstractBaseType {
        const bool value;
        explicit BooleanValue(bool value) : value(value) {}

        TYPENAME("boolean")
        STRING { return value ? "true" : "false"; }
    };

    struct NumberValue final : AbstractBaseType {
        const long double value;
        explicit NumberValue(long double value) : value(value) {}

        TYPENAME("number")
        STRING { return utils::formatNumber(value); }
    };

    struct StringValue final : AbstractBaseType {
        const std::string value;
        explicit StringValue(std::string &value) : value(value) {}

        TYPENAME("string")
        STRING { return value; }
    };


    struct ArrayObject final : AbstractBaseType {
        std::vector<SharedValue> value;
        explicit ArrayObject(std::vector<SharedValue> &value) : value(value) {}

        TYPENAME("array")
        STRING;
    };

    // TODO: Change it so i can do 'fun factorial(1)'
    class LexicalScope; // forward declaration to avoid cycle
    struct FunctionalObject final : AbstractBaseType {
        const std::vector<ExpressionPtr> &parameters;
        const BlockStatement &body;
        std::shared_ptr<LexicalScope> scope;
        FunctionalObject (
            std::vector<ExpressionPtr> &parameters,
            BlockStatement &body,
            std::shared_ptr<LexicalScope> &scope
        ) : parameters(parameters), body(body), scope(scope) {}

        TYPENAME("function")
        STRING;
    };

}

// TODO: enable math operations at least!