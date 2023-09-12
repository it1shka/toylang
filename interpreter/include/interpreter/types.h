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
// forward declaration to avoid cycles
namespace interpreter { class LexicalScope; }

#define TYPENAME(NAME)  [[nodiscard]] std::string getTypename() const override { return NAME; }
#define STRING          [[nodiscard]] std::string toString()    const override
#define DATA_TYPE(TYPE) [[nodiscard]] DataType    dataType()    const override { return TYPE; }

using namespace parser::AST;

namespace interpreter::types {

    struct AnyValue {
        enum class DataType {
            NilType,
            BooleanType,
            NumberType,
            StringType,
            ArrayType,
            FunctionType
        };
        [[nodiscard]] virtual DataType    dataType()    const = 0;
        [[nodiscard]] virtual std::string getTypename() const = 0;
        [[nodiscard]] virtual std::string toString()    const = 0;
    };

    using SharedValue = std::shared_ptr<AnyValue>;
    using enum AnyValue::DataType;

    struct NilValue final : AnyValue {
        DATA_TYPE(NilType)
        TYPENAME("nil")
        STRING { return "nil"; }
        static SharedValue getInstance();
    private:
        NilValue() = default;
    };

    struct BooleanValue final : AnyValue {
        const bool value;
        explicit BooleanValue(bool value) : value(value) {}

        DATA_TYPE(BooleanType)
        TYPENAME("boolean")
        STRING { return value ? "true" : "false"; }
    };

    struct NumberValue final : AnyValue {
        const long double value;
        explicit NumberValue(long double value) : value(value) {}

        DATA_TYPE(NumberType)
        TYPENAME("number")
        STRING { return utils::formatNumber(value); }
    };

    struct StringValue final : AnyValue {
        const std::string value;
        explicit StringValue(std::string &value) : value(value) {}

        DATA_TYPE(StringType)
        TYPENAME("string")
        STRING { return value; }
    };


    struct ArrayObject final : AnyValue {
        std::vector<SharedValue> value;
        explicit ArrayObject(std::vector<SharedValue> &value) : value(value) {}

        DATA_TYPE(ArrayType)
        TYPENAME("array")
        STRING;
    };

    struct FunctionalObject final : AnyValue {
        const std::vector<ExpressionPtr> &parameters;
        const StatementPtr &body;
        std::shared_ptr<LexicalScope> scope;
        FunctionalObject (
            const std::vector<ExpressionPtr> &parameters,
            const StatementPtr &body,
            std::shared_ptr<LexicalScope> &scope
        ) : parameters(parameters), body(body), scope(scope) {}

        DATA_TYPE(FunctionType)
        TYPENAME("function")
        STRING;
    };

    template <AnyValue::DataType expectedType, typename expectedValue>
    expectedValue* typeCast(const SharedValue& value);

}

// TODO: enable math operations at least!