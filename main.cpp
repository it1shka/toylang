#include <vector>
#include <iostream>
#include <sstream>
#include "parser/parser.h"

const std::string sample = R"(
        fun factorial (a) {
            let if
            let output = 1;
            for (i om 1 to a) {
                output *= i;
            }
            return output;
        }

fun main() {
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
