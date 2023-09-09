#pragma once
#include <stdexcept>
#include <string>

namespace interpreter::exceptions {
    class RuntimeException : public std::exception {};

    class UndefinedVariableException : public RuntimeException {
        const std::string message;
        static std::string createMessage(const std::string &varname) {
            return "Variable '" + varname + "' has not been defined yet."
        }
    public:
        explicit UndefinedVariableException(const std::string &varname)
            : message(createMessage(varname)) {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };
}