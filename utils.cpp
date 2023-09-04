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