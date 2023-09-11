#include "scope.h"
#include <utility>
#include "except.h"

using namespace interpreter;
using namespace interpreter::exceptions;
using interpreter::types::NilValue;

SharedScope LexicalScope::create() {
    return std::shared_ptr<LexicalScope>(new LexicalScope());
}

SharedScope LexicalScope::createInner(SharedScope &parent) {
    auto inner = LexicalScope::create();
    inner->parent = parent;
    return inner;
}

void LexicalScope::initVariable(const std::string &name, std::optional<SharedValue> value) {
    if (storage.find(name) != storage.end()) {
        throw CannotRedeclareException(name);
    }
    if (value.has_value()) {
        storage[name] = *value;
    } else {
        storage[name] = NilValue::getInstance();
    }
}

SharedValue LexicalScope::getValue(const std::string &name) {
    if (storage.find(name) != storage.end()) {
        return storage[name];
    }
    if (!parent.has_value()) {
        throw UndefinedVariableException(name);
    }
    return (*parent)->getValue(name);
}

void LexicalScope::setValue(const std::string &name, SharedValue &value) {
    if (storage.find(name) != storage.end()) {
        storage[name] = value;
        return;
    }
    if (!parent.has_value()) {
        throw UndefinedVariableException(name);
    }
    (*parent)->setValue(name, value);
}

std::optional<SharedScope> LexicalScope::getParent() {
    return parent;
}