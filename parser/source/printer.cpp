#include "printer.h"

using namespace parser;

Printer::Printer(unsigned tabSize)
    : tabSize(tabSize), tabLevel(0) {}

void Printer::increaseTabLevel() {
    tabLevel++;
}

void Printer::decreaseTabLevel() {
    tabLevel--;
}

void Printer::pad() {
    accumulate += std::string(tabLevel * tabSize, ' ');
}

Printer& Printer::operator<<(const std::string &append) {
    accumulate += append;
    return *this;
}

std::string Printer::getAccumulate() {
    return accumulate;
}
