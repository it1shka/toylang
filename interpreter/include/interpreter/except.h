#pragma once
#include <stdexcept>
#include <string>

namespace interpreter::exceptions {
    class RuntimeException : public std::exception {};

    class InternalException : public RuntimeException {
        const std::string message;
    public:
        explicit InternalException(const std::string &reason)
            : message("Internal exception: " + reason) {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };

    class UnimplementedException : public RuntimeException {
        const std::string message;
    public:
        explicit UnimplementedException(const std::string &functionality)
            : message("Unimplemented functionality: " + functionality) {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };

    class UndefinedVariableException : public RuntimeException {
        const std::string message;
        static std::string createMessage(const std::string &varname) {
            return "Variable '" + varname + "' has not been defined yet"
        }
    public:
        explicit UndefinedVariableException(const std::string &varname)
            : message(createMessage(varname)) {}
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };
}