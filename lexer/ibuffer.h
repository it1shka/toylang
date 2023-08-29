#pragma once
#include <string>
#include <optional>
#include <iostream>

namespace lexer {
    class InputBuffer {
        std::istream &source;
        std::optional<char> current;
        unsigned line, column;
    public:
        explicit InputBuffer(std::istream &source);
        char peek();
        char next();
        [[nodiscard]] bool eof() const;
        [[nodiscard]] std::tuple<unsigned, unsigned> getPosition() const;
    };
}