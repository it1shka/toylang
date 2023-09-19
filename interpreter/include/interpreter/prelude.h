#pragma once
#include <map>
#include "types.h"

namespace interpreter::prelude {
    const std::map<std::string, types::SharedValue>& getPrelude();
}