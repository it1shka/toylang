#pragma once
#include <tuple>

namespace AST {
    // Base struct for all AST constructs
    struct Node {
        const std::tuple<unsigned, unsigned> position;
    };

    // Two types 
    struct Statement  : Node {};
    struct Expression : Node {};

    struct Program {

    };
}