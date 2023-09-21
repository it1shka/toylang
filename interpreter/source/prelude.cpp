#include "prelude.h"
#include "except.h"
#include "types.h"
#include "utils/utils.h"
#include <memory>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>

#define NUMBER(VALUE) std::make_shared<NumberValue>(VALUE)
#define BOOL(VALUE)   std::make_shared<BooleanValue>(VALUE)
#define ARRAY(VALUE)  std::make_shared<ArrayObject>(VALUE)
#define STRING(VALUE) std::make_shared<StringValue>(VALUE)
#define NIL           NilValue::getInstance()
#define ARGS_SIZE(VALUE)    \
    if (args.size() != VALUE)   \
        throw exceptions::ParamsAndArgsDontMatchException(1, args.size());

using namespace interpreter::types;
using enum AnyValue::DataType;

const std::map<std::string, SharedValue>& interpreter::prelude::getPrelude() {
    const static std::map<std::string, SharedValue>& preludeMap {
            {"PI", NUMBER(3.14159265)},
            {"EXP", NUMBER(2.718)},
            {"size", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                return NUMBER(arrayPtr->value.size());
            })},
            {"abs", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto numberPtr = getCastedPointer<NumberType, NumberValue>(args[0]);
                const auto value = abs(numberPtr->value);
                return NUMBER(value);
            })},
            {"all", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                for (const auto &each : arrayPtr->value) {
                    const auto booleanPtr = getCastedPointer<BooleanType, BooleanValue>(each);
                    if (!booleanPtr->value) return BOOL(false);
                }
                return BOOL(true);
            })},
            {"any", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                for (const auto &each : arrayPtr->value) {
                    const auto booleanPtr = getCastedPointer<BooleanType, BooleanValue>(each);
                    if (booleanPtr->value) return BOOL(true);
                }
                return BOOL(false);
            })},
            {"print", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                for (const auto& each : args) {
                    std::cout << each->toString();
                }
                return NIL;
            })},
            {"println", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                for (const auto& each : args) {
                    std::cout << each->toString();
                }
                std::cout << std::endl;
                return NIL;
            })},
            {"array", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                auto arrayValues = args;
                return ARRAY(arrayValues);
            })},
            {"input", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                for (const auto& each : args) {
                    std::cout << each->toString();
                }
                std::string line;
                getline(std::cin, line);
                return STRING(line);
            })},
            {"bool", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                auto value = args[0];
                switch (value->dataType()) {
                    case NilType:
                        return BOOL(false);
                    case BooleanType:
                        return value;
                    case NumberType:
                        return BOOL(static_cast<NumberValue*>(value.get())->value == 1);
                    case StringType:
                        return BOOL(!static_cast<StringValue*>(value.get())->value.empty());
                    case ArrayType:
                        return BOOL(!static_cast<ArrayObject*>(value.get())->value.empty());
                    default:
                        return BOOL(true);
                }
            })},
            {"number", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                auto value = args[0];
                switch (value->dataType()) {
                    case BooleanType:
                        return NUMBER(static_cast<long double>(static_cast<BooleanValue*>(value.get())->value));
                    case NumberType:
                        return value;
                    case StringType: {
                        const auto str = static_cast<StringValue*>(value.get());
                        try {
                            const auto result = std::stold(str->value);
                            return NUMBER(result);
                        } catch (...) {
                            return NIL;
                        }
                    }
                    default:
                        return NIL;
                }
            })},
            {"max", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                if (arrayPtr->value.empty()) return NIL;
                auto maximal = arrayPtr->value[0];
                for (const auto& each : arrayPtr->value) {
                    const auto result = *each > maximal;
                    const auto boolResult = static_cast<BooleanValue*>(result.get());
                    if (boolResult->value) maximal = each;
                }
                return maximal;
            })},
            {"min", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                if (arrayPtr->value.empty()) return NIL;
                auto minimal = arrayPtr->value[0];
                for (const auto& each : arrayPtr->value) {
                    const auto result = *each < minimal;
                    const auto boolResult = static_cast<BooleanValue*>(result.get());
                    if (boolResult->value) minimal = each;
                }
                return minimal;
            })},
            // TODO: Handle edge cases and negative numbers
//            {"range", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
//                ARGS_SIZE(3)
//                const auto start = getCastedPointer<NumberType, NumberValue>(args[0])->value;
//                const auto end = getCastedPointer<NumberType, NumberValue>(args[1])->value;
//                const auto step = getCastedPointer<NumberType, NumberValue>(args[2])->value;
//                if (start < end && step <= 0) return NIL;
//                if (start > end && step >= 0) return NIL;
//                std::vector<SharedValue> rangeVector;
//                for (long double i = start; i < end; i += step) {
//                    rangeVector.push_back(NUMBER(i));
//                }
//                return ARRAY(rangeVector);
//            })},
            {"typeof", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                return STRING(args[0]->getTypename());
            })},
            {"str", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                return STRING(args[0]->toString());
            })},
            {"sum", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                if (arrayPtr->value.empty()) return NIL;
                auto output = arrayPtr->value[0];
                for (size_t i = 1; i < arrayPtr->value.size(); i++) {
                    *output += arrayPtr->value[i];
                }
                return output;
            })},
            {"slice", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(3)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                const auto start = getCastedPointer<NumberType, NumberValue>(args[1])->value;
                if (start < 0) return NIL;
                const auto end = getCastedPointer<NumberType, NumberValue>(args[2])->value;
                std::vector<SharedValue> newArray;
                for (size_t i = start; i < std::min(static_cast<size_t>(end), arrayPtr->value.size()); i++) {
                    newArray.push_back(arrayPtr->value[i]);
                }
                return ARRAY(newArray);
            })},
            {"reversed", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto arrayPtr = getCastedPointer<ArrayType, ArrayObject>(args[0]);
                std::vector<SharedValue> newArray(arrayPtr->value.size());
                for (size_t i = 0; i < arrayPtr->value.size(); i++) {
                    newArray[i] = arrayPtr->value[arrayPtr->value.size() - i - 1];
                }
                return ARRAY(newArray);
            })},
            {"read", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto name = getCastedPointer<StringType, StringValue>(args[0]);
                std::ifstream filestream(name->value);
                if (filestream.bad() || !filestream.is_open()) {
                    return NIL;
                }
                std::stringstream stream;
                stream << filestream.rdbuf();
                if (stream.bad()) {
                    return NIL;
                }
                return STRING(stream.str());
            })},
            {"write", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(2)
                const auto name = getCastedPointer<StringType, StringValue>(args[0]);
                const auto content = args[1];

                std::ofstream filestream(name->value, std::ios::out | std::ios::trunc);
                if (!filestream.is_open()) {
                    return BOOL(false);
                }

                filestream << content->toString();

                if (filestream.bad()) {
                    return BOOL(false);
                }

                filestream.close();
                return BOOL(true);
            })},
            {"round", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto number = getCastedPointer<NumberType, NumberValue>(args[0])->value;
                const auto rounded = round(number);
                return NUMBER(rounded);
            })},
            {"trunc", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto number = getCastedPointer<NumberType, NumberValue>(args[0])->value;
                const auto truncated = trunc(number);
                return NUMBER(truncated);
            })},
            {"keys", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto obj = getCastedPointer<ObjectType, UserObject>(args[0]);
                auto keys = utils::mapKeys(obj->value);
                std::vector<SharedValue> values(keys.size());
                for (size_t i = 0; i < keys.size(); i++) {
                    values[i] = STRING(keys[i]);
                }
                return ARRAY(values);
            })},
            {"values", std::make_shared<BuiltinFunction>([](auto args) -> SharedValue {
                ARGS_SIZE(1)
                const auto obj = getCastedPointer<ObjectType, UserObject>(args[0]);
                auto values = utils::mapValues(obj->value);
                return ARRAY(values);
            })},
            // TODO: complete the standard library
    };
    return preludeMap;
}