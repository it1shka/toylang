#include <vector>
#include <iostream>
#include <sstream>
#include "parser/parser.h"

auto getInput() -> std::string {
    std::string output, line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit") return "exit";
        if (line == "STOP") break;
        output += line + "\n";
    }
    return output;
}

int main() {
    while (true) {
        auto input = getInput();
        if (input == "exit") break;

        std::istringstream stream(input);
        auto parser = parser::Parser(stream);
        auto ast = parser.buildAST();
        std::cout << programToString(ast, 2) << std::endl;
        for (const auto &each : parser.getErrors()) {
            std::cout << each << std::endl;
        }

    }
}
