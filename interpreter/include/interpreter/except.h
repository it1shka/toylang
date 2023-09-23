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

    class UnsupportedBinaryOperationException : public RuntimeException {
        const std::string message;
    public:
        UnsupportedBinaryOperationException(const std::string& typeA, const std::string& typeB)
            : message("Unsupported binary operation between types '" + typeA + "' and '" + typeB + "'") {}
        ENABLE_WHAT
    };

    class UnsupportedPrefixOperationException : public RuntimeException {
        const std::string message;
    public:
        explicit UnsupportedPrefixOperationException(const std::string& typeName)
            : message("Unsupported unary operation for type '" + typeName + "'") {}
        ENABLE_WHAT
    };

    class UnsupportedOperatorException : public RuntimeException {
        const std::string message;
    public:
        explicit UnsupportedOperatorException(const std::string &op)
            : message("Unsupported operator: " + op) {}
        ENABLE_WHAT
    };

    class NonIntegerIndexException : public RuntimeException {
        WHAT_DECLARATION {
            return "Expected integer index";
        }
    };

    class ExpectedIdentifierException : public RuntimeException {
        WHAT_DECLARATION {
            return "Expected identifier on the left side of assignment";
        }
    };

    class FunctionParameterWrongFormatException : public RuntimeException {
        WHAT_DECLARATION {
            return "Function parameters can only be either a variable or variable = default value";
        }
    };

    class ParamsAndArgsDontMatchException : public RuntimeException {
        const std::string message;
    public:
        ParamsAndArgsDontMatchException(size_t paramsSize, size_t argsSize)
            : message("Expected " + std::to_string(paramsSize) + " argument(s), found " + std::to_string(argsSize)) {}
        ENABLE_WHAT
    };

    class IndexOutOfBoundsException : public RuntimeException {
        const std::string message;
    public:
        explicit IndexOutOfBoundsException(size_t index)
            : message("Index " + std::to_string(index) + " is out of bounds") {}
        ENABLE_WHAT
    };

    class WrongIndexAccessTargetException : public RuntimeException {
        const std::string message;
    public:
        explicit WrongIndexAccessTargetException(const std::string& name)
            : message("Expected array or object as target, found: " + name) {}
        ENABLE_WHAT
    };

    class NegativeArrayIndexException : public RuntimeException {
        WHAT_DECLARATION {
            return "Index of array cannot be negative";
        }
    };

    class MisplacedFlowOperator : public RuntimeException {
        const std::string message;
    public:
        explicit MisplacedFlowOperator(const std::string &opName)
            : message("Misplaced flow operator: " + opName) {}
        ENABLE_WHAT
    };

    class ZeroStepException : public RuntimeException {
        WHAT_DECLARATION {
            return "For loop step cannot be zero";
        }
    };

    class NegativeStepException : public RuntimeException {
        WHAT_DECLARATION {
            return "Step should be positive";
        }
    };

    class PositiveStepException : public RuntimeException {
        WHAT_DECLARATION {
            return "Step should be negative";
        }
    };

    class DuplicateParameterException : public RuntimeException {
        const std::string message;
    public:
        explicit DuplicateParameterException(const std::string& param)
            : message("Parameter '" + param + "' has been already declared") {}
        ENABLE_WHAT
    };

    class UnsetParametersException : public RuntimeException {
        const std::string message;
    public:
        explicit UnsetParametersException(const std::string& paramList)
            : message("Some parameters are left unset: " + paramList) {}
        ENABLE_WHAT
    };

    class FileImportFailedException : public RuntimeException {
        const std::string message;
    public:
        explicit FileImportFailedException(const std::string& libName)
            : message("Failed to open library \"" + libName + "\"") {}
        ENABLE_WHAT
    };

    class ImportParserException : public RuntimeException {
        const std::string message;
    public:
        ImportParserException(const std::string& libName, const std::string& errors)
            : message("Parser errors in imported lib \"" + libName + "\" were found: \n" + errors) {}
        ENABLE_WHAT
    };

    class ImportEvalException : public RuntimeException {
        const std::string message;
    public:
        ImportEvalException(const std::string& libName, const std::string& error)
                : message("Evaluation error in imported lib \"" + libName + "\" were found: \n" + error) {}
        ENABLE_WHAT
    };
}