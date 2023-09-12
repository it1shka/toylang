#include "types.h"
#include "except.h"

#define STRING_FOR(CLS) [[nodiscard]] std::string CLS::toString() const

using namespace interpreter::types;

template <AnyValue::DataType expectedType, typename expectedValue>
expectedValue* interpreter::types::typeCast(const SharedValue& value) {
    if (value->dataType() != expectedType) {
        throw exceptions::WrongTypeException(value->getTypename());
    }
    const auto pointer = value.get();
    return static_cast<expectedValue*>(pointer);
}

SharedValue NilValue::getInstance() {
    static auto singleton = std::shared_ptr<NilValue>(new NilValue());
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

// TODO: enable math operations at least!