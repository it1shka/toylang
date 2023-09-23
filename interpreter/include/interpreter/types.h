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
#include <utility>
#include <vector>
#include "parser/ast.h"
#include "utils/utils.h"
#include <map>
#include "except.h"
// forward declaration to avoid cycles
namespace interpreter { class LexicalScope; }

#define TYPENAME(NAME)  [[nodiscard]] std::string getTypename() const override { return NAME; }
#define DECL_STRING     [[nodiscard]] std::string toString()    const override
#define DATA_TYPE(TYPE) [[nodiscard]] DataType    dataType()    const override { return TYPE; }

using namespace parser::AST;

namespace interpreter::types {

    struct AnyValue {
        using SharedValue = std::shared_ptr<AnyValue>;
        enum class DataType {
            NilType,
            BooleanType,
            NumberType,
            StringType,
            ArrayType,
            FunctionType,
            ObjectType,
            BuiltinType,
        };
        [[nodiscard]] virtual DataType    dataType()    const = 0;
        [[nodiscard]] virtual std::string getTypename() const = 0;
        [[nodiscard]] virtual std::string toString()    const = 0;
        // operators
        // copy binary operators
        #define BIN_OP(OPERATOR) virtual SharedValue operator OPERATOR(const SharedValue &other) const;
        BIN_OP(||) BIN_OP(&&)
        BIN_OP(==) BIN_OP(!=)
        BIN_OP(< ) BIN_OP(> ) BIN_OP(<=) BIN_OP(>=)
        BIN_OP(+ ) BIN_OP(- )
        BIN_OP(* ) BIN_OP(/ ) BIN_OP(% ) BIN_OP(& ) // & is "div"
        BIN_OP(^ )
        // prefix operators
        #define PREF_OP(OPERATOR) virtual SharedValue operator OPERATOR() const;
        PREF_OP(!) PREF_OP(-)
        // mutate operators
        #define ASSIGN(OPERATOR) virtual void operator OPERATOR(const SharedValue &other);
        ASSIGN(+=) ASSIGN(-=)
        ASSIGN(*=) ASSIGN(/=)
        ASSIGN(^=)
    };

    #define OVERRIDE_BIN_OP(OPERATOR)  SharedValue operator OPERATOR(const SharedValue &other) const override;
    #define OVERRIDE_PREF_OP(OPERATOR) SharedValue operator OPERATOR()                   const override;
    #define OVERRIDE_ASSIGN(OPERATOR)  void        operator OPERATOR(const SharedValue &other)       override;

    using SharedValue = std::shared_ptr<AnyValue>;
    using enum AnyValue::DataType;

    template <AnyValue::DataType expectedType, typename expectedValue>
    expectedValue* getCastedPointer(const SharedValue& value) {
        if (value->dataType() != expectedType) {
            throw exceptions::WrongTypeException(value->getTypename());
        }
        const auto pointer = value.get();
        return static_cast<expectedValue*>(pointer);
    }

    SharedValue copyForAssignment(const SharedValue &value);

    struct NilValue final : AnyValue {
        DATA_TYPE(NilType)
        TYPENAME("nil")
        DECL_STRING { return "nil"; }
        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
        static SharedValue getInstance();
    private:
        NilValue() = default;
    };

    struct BooleanValue final : AnyValue {
        const bool value;
        explicit BooleanValue(bool value) : value(value) {}

        DATA_TYPE(BooleanType)
        TYPENAME("boolean")
        DECL_STRING { return value ? "true" : "false"; }

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
        OVERRIDE_BIN_OP(||) OVERRIDE_BIN_OP(&&)
        OVERRIDE_PREF_OP(!)
    };

    struct NumberValue final : AnyValue {
        long double value;
        explicit NumberValue(long double value) : value(value) {}

        DATA_TYPE(NumberType)
        TYPENAME("number")
        DECL_STRING { return utils::formatNumber(value); }

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
        OVERRIDE_BIN_OP(<)  OVERRIDE_BIN_OP(>) OVERRIDE_BIN_OP(<=) OVERRIDE_BIN_OP(>=)
        OVERRIDE_BIN_OP(+ ) OVERRIDE_BIN_OP(- )
        OVERRIDE_BIN_OP(* ) OVERRIDE_BIN_OP(/ ) OVERRIDE_BIN_OP(% ) OVERRIDE_BIN_OP(& )
        OVERRIDE_BIN_OP(^ )
        OVERRIDE_PREF_OP(-)
        OVERRIDE_ASSIGN(+=) OVERRIDE_ASSIGN(-=)
        OVERRIDE_ASSIGN(*=) OVERRIDE_ASSIGN(/=)
        OVERRIDE_ASSIGN(^=)
    };

    struct StringValue final : AnyValue {
        std::string value;
        explicit StringValue(std::string value) : value(std::move(value)) {}

        DATA_TYPE(StringType)
        TYPENAME("string")
        DECL_STRING { return value; }

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
        OVERRIDE_BIN_OP(< ) OVERRIDE_BIN_OP(> ) OVERRIDE_BIN_OP(<=) OVERRIDE_BIN_OP(>=)
        OVERRIDE_BIN_OP(+ ) OVERRIDE_BIN_OP(* )
        OVERRIDE_ASSIGN(+=) OVERRIDE_ASSIGN(*=)
    };

    struct ArrayObject final : AnyValue {
        std::vector<SharedValue> value;
        // I'm moving here -- watch out
        // not to use the argument after the constructor
        explicit ArrayObject(std::vector<SharedValue> &value) : value(std::move(value)) {}

        DATA_TYPE(ArrayType)
        TYPENAME("array")
        DECL_STRING;

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
        OVERRIDE_BIN_OP(+ ) OVERRIDE_BIN_OP(- )
        OVERRIDE_BIN_OP(* )
        OVERRIDE_ASSIGN(+=) OVERRIDE_ASSIGN(-=)
        OVERRIDE_ASSIGN(*=)
    };

    struct FunctionalObject final : AnyValue {
        const std::string filename;
        const std::vector<ExpressionPtr> &parameters;
        const StatementPtr &body;
        std::shared_ptr<LexicalScope> scope;
        FunctionalObject (
            std::string filename,
            const std::vector<ExpressionPtr> &parameters,
            const StatementPtr &body,
            std::shared_ptr<LexicalScope> &scope
        ) : filename(std::move(filename)), parameters(parameters), body(body), scope(scope) {}

        DATA_TYPE(FunctionType)
        TYPENAME("function")
        DECL_STRING;

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
    };

    struct UserObject final : AnyValue {
        std::map<std::string, SharedValue> value;
        explicit UserObject(std::map<std::string, SharedValue> value = {})
            : value(std::move(value)) {}

        DATA_TYPE(ObjectType)
        TYPENAME("object")
        DECL_STRING;

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
    };

    struct BuiltinFunction final : AnyValue {
        using CppFunction = std::function<auto (const std::vector<SharedValue>&) -> SharedValue>;
        const CppFunction cppCode;
        explicit BuiltinFunction (
            CppFunction function
        ) : cppCode(std::move(function)) {}

        DATA_TYPE(BuiltinType)
        TYPENAME("builtin")
        DECL_STRING;

        OVERRIDE_BIN_OP(==) OVERRIDE_BIN_OP(!=)
    };

}
