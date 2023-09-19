#include "prelude.h"

using namespace interpreter::types;
const std::map<std::string, SharedValue>& interpreter::prelude::getPrelude() {
    const static std::map<std::string, SharedValue>& preludeMap {

    };
    return preludeMap;
}