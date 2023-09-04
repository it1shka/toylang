#pragma once
#include <string>

namespace parser {
    class Printer {
        const unsigned tabSize;
        unsigned tabLevel;
        std::string accumulate;
    public:
        explicit Printer(unsigned tabSize);
        void increaseTabLevel();
        void decreaseTabLevel();
        void pad();
        Printer& operator<<(const std::string &append);
        std::string getAccumulate();
    };
}