#include <vector>
#include <iostream>
#include <sstream>
#include "parser/parser.h"

const std::string sample = R"(
        fun main() {
            let start = 1;
            let end = 10;
            for (i from start to end step 2) {
                print(i)
            }
        }
        main();
    )";

int main() {
    std::istringstream stream(sample);
    auto parser = parser::Parser(stream);
    parser.buildAST();
    for (const auto &each : parser.getErrors()) {
        std::cout << each << std::endl;
    }
}
