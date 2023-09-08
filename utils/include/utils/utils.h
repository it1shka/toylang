#pragma once
#include <string>

namespace utils {
    std::string formatNumber(long double number);
    void stringReplace(std::string &source, const std::string &original, const std::string &replacement);
    std::string quotedString(const std::string &source, const std::string &quote);
}