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
        void initVariable(const std::string &name, std::optional<SharedValue> value = std::nullopt);
        [[nodiscard]] SharedValue getValue(const std::string &name);
        void setValue(const std::string &name, SharedValue &value);
        [[nodiscard]] std::optional<SharedScope> getParent();
    };

    using SharedScope = std::shared_ptr<LexicalScope>;
    using Storage = std::map<std::string, SharedValue>;
}