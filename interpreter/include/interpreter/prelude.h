#pragma once
#include <map>
#include "types.h"

namespace interpreter::prelude {
    using namespace interpreter::types;
    const std::map<std::string, SharedValue>& getPrelude();
}