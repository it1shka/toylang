#pragma once
#include <string>
#include <cmath>

namespace utils {
    std::string formatNumber(long double number);
    void stringReplace(std::string &source, const std::string &original, const std::string &replacement);
    std::string quotedString(const std::string &source, const std::string &quote);
    bool isInteger(long double number, long double tolerance = 1e-9);
}