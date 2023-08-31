#include <vector>
#include <iostream>
#include <sstream>
#include "lexer/lexer.h"
#include "parser/ast.h"

auto testLexer() -> void {
    using namespace lexer;

    const std::string text = R"(
        fun factorial(n) {
            let output = 1;
            for (i from 1 to n) {
                output *= i;
            }
            return output;
        }

        let user_input = get_number();
        let result = factorial(user_input);
        print(result);
    )";

    auto stream = std::istringstream(text);
    auto lexer = Lexer(stream);

    while (!lexer.eof()) {
        auto token = lexer.next();
        std::cout << token.toString() << std::endl;
    }
}

int main() {
    testLexer();
}
