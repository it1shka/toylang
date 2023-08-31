#include "lexer.h"
#include <string>
#include <set>

using namespace lexer;

const std::set<std::string> KEYWORDS {
    "let",                          // variable declaration
    "for", "from", "to", "step",    // for loop
    "while",                        // while loop
    "continue", "break",            // loop flow operators
    "if", "else",                   // if-else
    "fun", "lambda",                // functions
    "return",                       // return value from functions
    "true", "false",                // boolean literals
};

const std::set<std::string> OPERATORS {
    "=",
    "or", "and",
    "==", "!=",
    "<", ">", "<=", ">=",
    "-", "+", "*", "/", "mod",
    "not",
};

const std::set<char> PUNCTUATION {
    '(', ')', '{', '}', ',', ';'
};

// public methods
Lexer::Lexer(std::istream &source) : buffer(InputBuffer(source)) {
    currentToken = std::nullopt;
    currentPosition = buffer.getPosition();
}

const Token& Lexer::peek() {
    if (!currentToken) {
        const auto token = readToken();
        currentToken.emplace(token);
    }
    return *currentToken;
}

Token Lexer::next() {
    if (!currentToken) {
        return readToken();
    }
    auto token = *currentToken;
    currentToken = std::nullopt;
    return token;
}

bool Lexer::eof() {
    return peek().type == TokenType::EndOfFile;
}

// private methods
Token Lexer::readToken() {
    using enum TokenType;

    // Skipping whitespace and setting position pointer
    skipWhitespace();
    currentPosition = buffer.getPosition();

    // Handling EOF
    if (buffer.eof()) {
        return makeToken(EndOfFile, "");
    }

    const auto current = buffer.peek();

    // Skip comments
    if (current == '#') {
        skipComment();
        return readToken();
    }

    // words (keywords, identifiers, some of operators)
    if (isalpha(static_cast<int>(current)) || current == '_') {
        return readWordToken();
    }

    // numbers
    if (isdigit(static_cast<int>(current))) {
        return readNumberToken();
    }

    // will be used to create punctuation / non-alpha ops
    // initially is empty (empty string = "")
    std::string value;

    // Punctuation (){},;
    if (PUNCTUATION.contains(current)) {
        value += buffer.next();
        return makeToken(Punctuation, value);
    }

    // Non-alpha operators
    switch(current) {
        case '=': case '+': case '-':
        case '*': case '/': case '>': case '<':
            value += buffer.next();
            if (buffer.peek() == '=') value += buffer.next();
            return makeToken(Operator, value);
        case '!':
            if (buffer.peek() != '=') return makeToken(Illegal, value);
            return makeToken(Operator, value + buffer.next());
        default:
            // finally, if nothing clicks, return illegal
            return readIllegalToken();
    }
}

Token Lexer::readIllegalToken() {
    const auto value = readWhile([](auto ch) {
        const auto result = isspace(static_cast<int>(ch));
        return !static_cast<bool>(result);
    });
    return makeToken(TokenType::Illegal, value);
}

Token Lexer::readWordToken() {
    const auto value = readWhile([](auto ch) {
        const auto isAlphanum = std::isalnum(static_cast<int>(ch));
        return static_cast<bool>(isAlphanum) || ch == '_';
    });
    using enum TokenType;
    if (KEYWORDS.contains(value)) {
        return makeToken(Keyword, value);
    }
    if (OPERATORS.contains(value)) {
        return makeToken(Operator, value);
    }
    return makeToken(Identifier, value);
}

Token Lexer::readNumberToken() {
    const auto readDigits = [this]() {
        return readWhile([](auto ch) {
            const auto isDigit = std::isdigit(static_cast<int>(ch));
            return static_cast<bool>(isDigit);
        });
    };
    auto value = readDigits();
    if (buffer.peek() == '.') {
        value += buffer.next();
        value += readDigits();
    }
    return makeToken(TokenType::Number, value);
}

Token Lexer::makeToken(TokenType type, std::string value) const {
    return {
        type,std::move(value),
        currentPosition
    };
}

void Lexer::skipWhile(const std::function<bool(char)> &predicate) {
    while(!buffer.eof() && predicate(buffer.peek())) {
        buffer.next();
    }
}

void Lexer::skipWhitespace() {
    skipWhile([] (auto ch) {
        const auto result = isspace(static_cast<int>(ch));
        return static_cast<bool>(result);
    });
}

void Lexer::skipComment() {
    skipWhile([] (auto ch) {
       return ch != '\n';
    });
}

std::string Lexer::readWhile(const std::function<bool(char)> &predicate) {
    std::string output;
    while (!buffer.eof() && predicate(buffer.peek())) {
        const auto letter = buffer.next();
        output += letter;
    }
    return output;
}
