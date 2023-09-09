#include "scope.h"
#include <utility>
#include "except.h"

using namespace interpreter;
using interpreter::exceptions::UndefinedVariableException;
using interpreter::types::NilValue;

SharedScope LexicalScope::create() {
    return std::shared_ptr<LexicalScope>(new LexicalScope());
}

SharedScope LexicalScope::createInner(SharedScope &parent) {
    auto inner = LexicalScope::create();
    inner->parent = parent;
    return inner;
}

SharedValue LexicalScope::getValue(const std::string &name) {
    if (storage.find(name) != storage.end()) {
        return storage[name];
    }
    if (!parent.has_value()) {
        return NilValue::getInstance();
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