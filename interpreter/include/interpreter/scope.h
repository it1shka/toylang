#pragma once
#include <optional>
#include <map>
#include <memory>
#include "types.h"

using interpreter::types::SharedValue;

namespace interpreter {
    class LexicalScope final {
        using SharedScope = std::shared_ptr<LexicalScope>;
        std::optional<SharedScope> parent;
        std::map<std::string, SharedValue> storage;
        LexicalScope() : parent(std::nullopt) {}
    public:
        static SharedScope create();
        static SharedScope createInner(SharedScope &parent);
        [[nodiscard]] SharedValue getValue(const std::string &name);
        void setValue(const std::string &name, SharedValue &value);
    };

    using SharedScope = std::shared_ptr<LexicalScope>;

}