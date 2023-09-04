#include <vector>
#include <iostream>
#include <sstream>
#include "parser/parser.h"

const std::string sample = R"(
        fun sum(a, b) {
            let output = 0.1120000;
            for (_i from a to b) {
                output += _i;
            }
            return output;
        }
    )";

int main() {
    std::istringstream stream(sample);
    auto parser = parser::Parser(stream);
    auto ast = parser.buildAST();
    for (const auto &each : parser.getErrors()) {
        std::cout << each << std::endl;
    }
    std::cout << parser::AST::programToString(ast, 4);
}
