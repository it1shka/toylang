#include <vector>
#include <iostream>
#include <sstream>
#include "parser/parser.h"
#include "interpreter/interpreter.h"

auto getInput() -> std::string {
    std::string output, line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit") return "exit";
        if (line == "FEED") break;
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
        auto ast = parser.readProgram();
        if (!parser.getErrors().empty()) {
            for (const auto &each : parser.getErrors()) {
                std::cout << each << std::endl;
            }
            continue;
        }

        auto interpreter = interpreter::Interpreter();
        interpreter.executeProgram(*ast);
        if (interpreter.didFailed()) {
            std::cout << *interpreter.getFatalError() << std::endl;
        }
        for (const auto &each : interpreter.getWarnings()) {
            std::cout << each << std::endl;
        }
        std::cout << std::endl;
    }
}
