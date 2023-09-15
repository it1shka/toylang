#include "utils.h"

std::string utils::formatNumber(long double number) {
    auto output = std::to_string(number);
    auto decimalPos = output.find('.');
    if (decimalPos == std::string::npos) {
        return output;
    }
    auto nonZeroPos = output.find_first_not_of('0', decimalPos + 1);
    if (nonZeroPos != std::string::npos) {
        output.erase(output.find_last_not_of('0') + 1);
    } else {
        output.erase(decimalPos);
    }
    return output;
}

void utils::stringReplace(std::string &source, const std::string &original, const std::string &replacement) {
    auto pointer = source.find(original);
    while(pointer != std::string::npos) {
        source.replace(pointer, original.size(), replacement);
        pointer += replacement.size();
        pointer = source.find(original, pointer);
    }
}

std::string utils::quotedString(const std::string &source, const std::string &quote) {
    const auto escapedQuote = "\\" + quote;
    auto output = source;
    stringReplace(output, quote, escapedQuote);
    stringReplace(output, "\t", "\\t");
    stringReplace(output, "\n", "\\n");
    return quote + output + quote;
}

bool utils::isInteger(long double value, long double tolerance) {
    long double truncatedValue = std::trunc(value);
    return std::abs(value - truncatedValue) < tolerance;
}