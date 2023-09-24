#include <queue>
#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "parser/parser.h"
#include "interpreter/interpreter.h"

class ArgumentReader {
    std::queue<std::string> arguments;
    class ArgumentException : public std::exception {
        const std::string message;
    public:
        explicit ArgumentException(const std::string& argName)
            : message("Expected argument \"" + argName + "\"") {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };
public:
    ArgumentReader(int argc, char* argv[]) {
        for (size_t i = 1; i < argc; i++) {
            arguments.push(argv[i]);
        }
    }
    auto readIf(const std::string& value) -> bool {
        if (arguments.empty()) return false;
        if (arguments.front() == value) {
            arguments.pop();
            return true;
        }
        return false;
    }
    auto read(const std::string& argName) -> std::string {
        if (arguments.empty()) {
            throw ArgumentException(argName);
        }
        auto argument = arguments.front();
        arguments.pop();
        return argument;
    }
};

auto executeCode(const std::string& filename, std::istream& stream) -> void {
    parser::Parser _parser(stream);
    const auto ast = _parser.readProgram();
    if (!_parser.getErrors().empty()) {
        std::cerr << "Encountered errors while parsing: " << std::endl;
        for (const auto &each : _parser.getErrors()) {
            std::cerr << each << std::endl;
        }
        return;
    }
    interpreter::Interpreter _interpreter(filename);
    _interpreter.executeProgram(*ast);
    const auto maybeError = _interpreter.getFatalError();
    if (maybeError.has_value()) {
        std::cerr << std::endl << "Encountered a fatal error during runtime: "  << std::endl;
        std::cerr << maybeError.value() << std::endl;
    }
}

auto runConsole() -> void {
    std::string buffer, code;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, buffer);
        if (buffer == "EXIT") return;
        if (buffer == "EXEC") {
            std::istringstream stream(code);
            executeCode("CONSOLE", stream);
            code.clear();
            continue;
        }
        code += buffer + '\n';
    }
}

auto runFile(const std::string& filename) -> void {
    std::fstream filestream(filename);
    if (!filestream.good()) {
        std::cerr << "Error while opening file \"" << filename << "\". Maybe file does not exist" << std::endl;
        return;
    }
    executeCode(filename, filestream);
}

auto formatFile(const std::string& filename) -> void {
    std::ifstream filestream(filename);
    if (!filestream.good()) {
        std::cerr << "Error while opening file \"" << filename << "\". Maybe file does not exist" << std::endl;
        return;
    }
    parser::Parser _parser(filestream);
    const auto ast = _parser.readProgram();
    if (!_parser.getErrors().empty()) {
        std::cerr << "Found some errors while parsing: " << std::endl << std::endl;
        for (const auto &each : _parser.getErrors()) {
            std::cerr << each << std::endl;
        }
        return;
    }
    filestream.close();
    const auto newText = ast->toFormatString();
    std::ofstream outputStream(filename);
    if (outputStream.is_open()) {
        outputStream << newText;
    } else {
        std::cerr << "Error while overwriting file \"" << filename << "\"" << std::endl;
    }
}

auto showHelp() -> void {
    const auto information = R"(Welcome to Toylang!
The interpreter command-line application has
multiple commands for you to utilize:

1) help
    [usage: toylang help]
    Shows help information about the
    interpreter console application

2) console
    [usage: toylang console]
    You can write code in the console.
    When you are ready to execute
    what you've written above,
    just type on a single empty line "EXEC".
    When you want to exit, write "EXIT" on
    a new empty line

3) run
    [usage: toylang run <filename>]
    Runs code you provided in a particular file
    under the name <filename>

4) format
    [usage: toylang format <filename>]
    Formats code in a file under the
    name <filename>
)";
    std::cout << information;
}

auto main(int argc, char* argv[]) -> int {
    auto reader = ArgumentReader(argc, argv);
    if (reader.readIf("help")) {
        showHelp();
        return 0;
    }
    if (reader.readIf("console")) {
        runConsole();
        return 0;
    }
    if (reader.readIf("run")) {
        const auto filename = reader.read("filename");
        runFile(filename);
        return 0;
    }
    if (reader.readIf("format")) {
        const auto filename = reader.read("filename");
        formatFile(filename);
        return 0;
    }
    std::cout << "No arguments were provided, showing help: " << std::endl << std::endl;
    showHelp();
}