#include <vector>
#include <iostream>
#include <sstream>
#include "parser/parser.h"

const std::string sample = R"(
        fun main() {
            let a = 1 + 2 * 3 + 4 + (5 * 6 ^ 7);
            print(a);
        }

        main();
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
