#pragma once
#include <stdexcept>
#include <string>

#define WHAT_DECLARATION [[nodiscard]] const char* what() const noexcept override
#define ENABLE_WHAT WHAT_DECLARATION { return message.c_str(); }

namespace interpreter::exceptions {
    class RuntimeException : public std::exception {};

    class InternalException : public RuntimeException {
        const std::string message;
    public:
        explicit InternalException(const std::string &reason)
            : message("Internal exception: " + reason) {}
        ENABLE_WHAT
    };

    class UnimplementedException : public RuntimeException {
        const std::string message;
    public:
        explicit UnimplementedException(const std::string &functionality)
            : message("Unimplemented functionality: " + functionality) {}
        ENABLE_WHAT
    };

    class UndefinedVariableException : public RuntimeException {
        const std::string message;
        static std::string createMessage(const std::string &varname) {
            return "Variable '" + varname + "' has not been defined yet";
        }
    public:
        explicit UndefinedVariableException(const std::string &varname)
            : message(createMessage(varname)) {}
        ENABLE_WHAT
    };

    class CannotRedeclareException : public RuntimeException {
        const std::string message;
    public:
        explicit CannotRedeclareException(const std::string &term)
            : message("Cannot redeclare " + term) {}
        ENABLE_WHAT
    };

    class PropagatedException : public RuntimeException {
        const std::string message;
    public:
        PropagatedException(const std::string &label, const std::string &old)
            : message("At " + label + ":\n" + old) {}
        ENABLE_WHAT
    };

    class ErrorNodeException : public RuntimeException {
    public:
        WHAT_DECLARATION {
            return "Cannot execute error node";
        }
    };

    class WrongTypeException : public RuntimeException {
        const std::string message;
    public:
        explicit WrongTypeException(const std::string &typeName)
            : message("Unexpected type '" + typeName + "'") {}
        ENABLE_WHAT
    };

    class UnsupportedBinaryOperation : public RuntimeException {
        const std::string message;
    public:
        UnsupportedBinaryOperation(const std::string& typeA, const std::string& typeB)
            : message("Unsupported binary operation between types '" + typeA + "' and '" + typeB + "'") {}
        ENABLE_WHAT
    };

    class UnsupportedPrefixOperation : public RuntimeException {
        const std::string message;
    public:
        explicit UnsupportedPrefixOperation(const std::string& typeName)
            : message("Unsupported unary operation for type '" + typeName + "'") {}
        ENABLE_WHAT
    };

    class UnsupportedOperator : public RuntimeException {
        const std::string message;
    public:
        explicit UnsupportedOperator(const std::string &op)
            : message("Unsupported operator: " + op) {}
        ENABLE_WHAT
    };

    class NonIntegerIndex : public RuntimeException {
        WHAT_DECLARATION {
            return "Expected integer index";
        }
    };
}