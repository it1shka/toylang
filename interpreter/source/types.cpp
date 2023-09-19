#include "types.h"
#include "except.h"
#include <cmath>

#define STRING_FOR(CLS) [[nodiscard]] std::string CLS::toString() const

using namespace interpreter::types;

// copying mechanism

SharedValue interpreter::types::copyForAssignment(const SharedValue &value) {
    #define COPY_VALUE(TYPE)                                  \
        return std::shared_ptr<TYPE> (                        \
            new TYPE (                                        \
                static_cast<const TYPE*>(value.get())->value  \
            )                                                 \
        );

    switch (value->dataType()) {
        case ArrayType: case FunctionType: case BuiltinType:
            return value;
        case NilType:
            return NilValue::getInstance();
        case BooleanType:
            COPY_VALUE(BooleanValue)
        case NumberType:
            COPY_VALUE(NumberValue)
        case StringType:
            COPY_VALUE(StringValue)
    }
}

SharedValue NilValue::getInstance() {
    static const auto singleton = std::shared_ptr<NilValue>(new NilValue());
    return singleton;
}

STRING_FOR(ArrayObject) {
    auto output = std::string("[");
    for (size_t i = 0; i < value.size(); i++) {
        const auto elem = value[i];
        output += elem->toString();
        if (i != value.size() - 1) {
            output += ", ";
        }
    }
    return output + "]";
}

STRING_FOR(FunctionalObject) {
    auto output = std::string("function (");
    for (size_t i = 0; i < parameters.size(); i++) {
        output += parameters[i]->toFormatString();
        if (i != parameters.size() - 1) {
            output += ", ";
        }
    }
    output += ")";
    output += body->toFormatString();
    return output;
}

STRING_FOR(BuiltinFunction) {
    return "built-in";
}

// OPERATORS IMPLEMENTATION
using namespace interpreter::exceptions;
#define BIN_OP_FOR(CLS, OP)  SharedValue CLS::operator OP(const SharedValue &other) const
#define PREF_OP_FOR(CLS, OP) SharedValue CLS::operator OP()                   const
#define ASSIGN_FOR(CLS, OP)  void        CLS::operator OP(const SharedValue &other)

#define UNSUPPORTED_BIN_OP {                         \
    const auto typeA = getTypename();                \
    const auto typeB = other->getTypename();         \
    throw UnsupportedBinaryOperationException(typeA, typeB);  \
}

#define UNSUPPORTED_PREF_OP {                   \
    const auto typeName = getTypename();        \
    throw UnsupportedPrefixOperationException(typeName); \
}

// AnyValue -- all operations are not supported
BIN_OP_FOR(AnyValue, ||) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, &&) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, ==) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, !=) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, < ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, > ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, <=) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, >=) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, + ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, - ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, * ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, / ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, % ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, & ) UNSUPPORTED_BIN_OP
BIN_OP_FOR(AnyValue, ^ ) UNSUPPORTED_BIN_OP
PREF_OP_FOR(AnyValue, !) UNSUPPORTED_PREF_OP
PREF_OP_FOR(AnyValue, -) UNSUPPORTED_PREF_OP
ASSIGN_FOR(AnyValue, +=) UNSUPPORTED_BIN_OP
ASSIGN_FOR(AnyValue, -=) UNSUPPORTED_BIN_OP
ASSIGN_FOR(AnyValue, *=) UNSUPPORTED_BIN_OP
ASSIGN_FOR(AnyValue, /=) UNSUPPORTED_BIN_OP
ASSIGN_FOR(AnyValue, ^=) UNSUPPORTED_BIN_OP

#define SHARED_BOOL(VALUE)   std::make_shared<BooleanValue>(VALUE)
#define SHARED_NUMBER(VALUE) std::make_shared<NumberValue>(VALUE)
#define SHARED_STRING(VALUE) std::make_shared<StringValue>(VALUE)
#define SHARED_ARRAY(VALUE)  std::make_shared<ArrayObject>(VALUE)

#define NON_EQUAL_TYPES_BOOL(VALUE)                   \
    if (dataType() != other->dataType()) {            \
        return SHARED_BOOL(VALUE);                    \
    }

#define EQUAL_TYPES_BOOL(VALUE)                       \
    if (dataType() == other->dataType()) {            \
        return SHARED_BOOL(VALUE);                    \
    }

#define UNCHECKED_CASTED_OTHER(VALUE)    const auto castedOther = static_cast<VALUE*>(other.get());
#define CHECKED_CASTED_OTHER(TYPE,VALUE) const auto castedOther = getCastedPointer<TYPE,VALUE>(other);

#define DEFAULT_EQ(VALUE) {                          \
    NON_EQUAL_TYPES_BOOL(false)                      \
    UNCHECKED_CASTED_OTHER(VALUE)                    \
    return SHARED_BOOL(value == castedOther->value); \
}

#define DEFAULT_NEQ(VALUE) {                          \
    NON_EQUAL_TYPES_BOOL(true)                        \
    UNCHECKED_CASTED_OTHER(VALUE)                     \
    return SHARED_BOOL(value != castedOther->value);  \
}

// NilValue -- only == and != are supported
BIN_OP_FOR(NilValue, ==) {
    EQUAL_TYPES_BOOL(true)
    return SHARED_BOOL(false);
}

BIN_OP_FOR(NilValue, !=) {
    NON_EQUAL_TYPES_BOOL(true)
    return SHARED_BOOL(false);
}

// BooleanValue -- bool operators
BIN_OP_FOR(BooleanValue, ==) DEFAULT_EQ(BooleanValue)

BIN_OP_FOR(BooleanValue, !=) DEFAULT_NEQ(BooleanValue)

BIN_OP_FOR(BooleanValue, ||) {
    CHECKED_CASTED_OTHER(BooleanType, BooleanValue)
    return SHARED_BOOL(value || castedOther->value);
}

BIN_OP_FOR(BooleanValue, &&) {
    CHECKED_CASTED_OTHER(BooleanType, BooleanValue)
    return SHARED_BOOL(value && castedOther->value);
}

PREF_OP_FOR(BooleanValue, !) {
    return SHARED_BOOL(!value);
}

// NumberValue -- math operations + mut math operations + comparisons
BIN_OP_FOR(NumberValue, ==) DEFAULT_EQ(NumberValue)

BIN_OP_FOR(NumberValue, !=) DEFAULT_NEQ(NumberValue)

#define NUMBER_CMP(OP)                                   \
    BIN_OP_FOR(NumberValue, OP) {                        \
        CHECKED_CASTED_OTHER(NumberType, NumberValue)    \
        return SHARED_BOOL(value OP castedOther->value); \
    }

NUMBER_CMP(>)

NUMBER_CMP(<)

NUMBER_CMP(>=)

NUMBER_CMP(<=)

#define NUMBER_MATH(OP)                                    \
    BIN_OP_FOR(NumberValue, OP) {                          \
        CHECKED_CASTED_OTHER(NumberType, NumberValue);     \
        return SHARED_NUMBER(value OP castedOther->value); \
    }

NUMBER_MATH(+)

NUMBER_MATH(-)

NUMBER_MATH(*)

NUMBER_MATH(/)

BIN_OP_FOR(NumberValue, %) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    return SHARED_NUMBER(fmod(value, castedOther->value));
}

BIN_OP_FOR(NumberValue, &) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    const auto result = value / castedOther->value;
    const auto intResult = static_cast<long long>(result);
    return SHARED_NUMBER(static_cast<long double>(intResult));
}

BIN_OP_FOR(NumberValue, ^) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    const auto result = pow(value, castedOther->value);
    return SHARED_NUMBER(result);
}

PREF_OP_FOR(NumberValue, -) {
    return SHARED_NUMBER(-value);
}

#define NUMBER_MUT_MATH(OP)                           \
    ASSIGN_FOR(NumberValue, OP) {                     \
        CHECKED_CASTED_OTHER(NumberType, NumberValue) \
        value OP castedOther->value;                  \
    }

NUMBER_MUT_MATH(+=)

NUMBER_MUT_MATH(-=)

NUMBER_MUT_MATH(*=)

NUMBER_MUT_MATH(/=)

ASSIGN_FOR(NumberValue, ^=) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    const auto result = pow(value, castedOther->value);
    value = result;
}

// StringValue, compare, eq/neq, addition and multiplication
BIN_OP_FOR(StringValue, ==) DEFAULT_EQ(StringValue)

BIN_OP_FOR(StringValue, !=) DEFAULT_NEQ(StringValue)

#define STRING_CMP(OP)                                   \
    BIN_OP_FOR(StringValue, OP) {                        \
        CHECKED_CASTED_OTHER(StringType, StringValue)    \
        return SHARED_BOOL(value OP castedOther->value); \
    }

STRING_CMP(<)

STRING_CMP(>)

STRING_CMP(<=)

STRING_CMP(>=)

BIN_OP_FOR(StringValue, +) {
    return SHARED_STRING(value + other->toString());
}

BIN_OP_FOR(StringValue, *) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    std::string newValue;
    for (auto i = 0; i < castedOther->value; i++) {
        newValue += value;
    }
    return SHARED_STRING(newValue);
}

ASSIGN_FOR(StringValue, +=) {
    value += other->toString();
}

ASSIGN_FOR(StringValue, *=) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    std::string newValue;
    for (auto i = 0; i < castedOther->value; i++) {
        newValue += value;
    }
    value = newValue;
}

// ArrayObject -- deep eq/neq, add, subtract, multiply
BIN_OP_FOR(ArrayObject, ==) {
    NON_EQUAL_TYPES_BOOL(false)
    UNCHECKED_CASTED_OTHER(ArrayObject)
    if (value.size() != castedOther->value.size()) {
        return SHARED_BOOL(false);
    }
    for (size_t i = 0; i < value.size(); i++) {
        const auto mineValue = value[i];
        const auto otherValue = castedOther->value[i];
        const auto result = static_cast<BooleanValue*>((*mineValue == otherValue).get());
        if (!result->value) {
            return SHARED_BOOL(false);
        }
    }
    return SHARED_BOOL(true);
}

BIN_OP_FOR(ArrayObject, !=) {
    NON_EQUAL_TYPES_BOOL(true)
    UNCHECKED_CASTED_OTHER(ArrayObject)
    if (value.size() != castedOther->value.size()) {
        return SHARED_BOOL(true);
    }
    for (size_t i = 0; i < value.size(); i++) {
        const auto mineValue = value[i];
        const auto otherValue = castedOther->value[i];
        const auto result = static_cast<BooleanValue*>((*mineValue == otherValue).get());
        if (!result->value) {
            return SHARED_BOOL(true);
        }
    }
    return SHARED_BOOL(false);
}

BIN_OP_FOR(ArrayObject, +) {
    auto newValue = value;
    newValue.push_back(other);
    return SHARED_ARRAY(newValue);
}

BIN_OP_FOR(ArrayObject, *) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    std::vector<SharedValue> newValue;
    for (auto i = 0; i < castedOther->value; i++) {
        for (const auto &each : value) {
            newValue.push_back(each);
        }
    }
    return SHARED_ARRAY(newValue);
}

ASSIGN_FOR(ArrayObject, +=) {
    value.push_back(other);
}

ASSIGN_FOR(ArrayObject, *=) {
    CHECKED_CASTED_OTHER(NumberType, NumberValue)
    const auto oldValue = std::move(value);
    value = {};
    for (auto i = 0; i < castedOther->value; i++) {
        for (const auto &each : oldValue) {
            value.push_back(each);
        }
    }
}

ASSIGN_FOR(ArrayObject, -=) {
    const auto oldValue = std::move(value);
    value = {};
    for (auto &each : oldValue) {
        const auto boolValue = static_cast<BooleanValue*>((*each != other).get())->value;
        if (boolValue) value.push_back(each);
    }
}

BIN_OP_FOR(ArrayObject, -) {
    std::vector<SharedValue> next;
    for (auto &each : value) {
        const auto boolValue = static_cast<BooleanValue*>((*each != other).get())->value;
        if (boolValue) next.push_back(each);
    }
    return SHARED_ARRAY(next);
}

// FunctionalObject -- pointer eq/neq
BIN_OP_FOR(FunctionalObject, ==) {
    NON_EQUAL_TYPES_BOOL(false)
    return SHARED_BOOL(this == other.get());
}

BIN_OP_FOR(FunctionalObject, !=) {
    NON_EQUAL_TYPES_BOOL(true)
    return SHARED_BOOL(this != other.get());
}

BIN_OP_FOR(BuiltinFunction, ==) {
    NON_EQUAL_TYPES_BOOL(false)
    return SHARED_BOOL(this == other.get());
}

BIN_OP_FOR(BuiltinFunction, !=) {
    NON_EQUAL_TYPES_BOOL(true)
    return SHARED_BOOL(this != other.get());
}