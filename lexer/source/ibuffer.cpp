#include "ibuffer.h"

using namespace lexer;

InputBuffer::InputBuffer(std::istream& source) : source(source) {
    current = std::nullopt;
    line = 1;
    column = 1;
}

char InputBuffer::peek() {
    if (eof()) return '\0';
    if (current) return *current;

    char value;
    source.get(value);
    current = value;
    return value;
}

char InputBuffer::next() {
    const auto prev = peek();
    current = std::nullopt;
    if (prev == '\n') {
        line++;
        column = 1;
    } else column++;
    return prev;
}

bool InputBuffer::eof() const {
    return source.eof();
}

std::tuple<unsigned, unsigned> InputBuffer::getPosition() const {
    return { line, column };
}