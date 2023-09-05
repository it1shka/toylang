#include "lexer.h"
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
    "-", "+", "*", "/",
    "div", "mod",
    "^",
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
        auto token = readToken();
        currentToken.emplace(std::move(token));
    }
    return *currentToken;
}

Token Lexer::next() {
    if (!currentToken) {
        return readToken();
    }
    auto token = std::move(*currentToken);
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

    // will be user to create tokens later
    // buffer for valueBuffer of current token
    std::string valueBuffer;

    // Handling EOF
    if (buffer.eof()) {
        return makeToken(EndOfFile, valueBuffer);
    }

    const auto current = buffer.peek();

    // Skip comments
    if (current == '#') {
        skipComment();
        return readToken();
    }

    // words (keywords, identifiers, some of operators)
    if (isalpha(static_cast<int>(current)) || current == '_') {
        return readWordToken(valueBuffer);
    }

    // numbers
    if (isdigit(static_cast<int>(current))) {
        return readNumberToken(valueBuffer);
    }

    // Punctuation (){},;
    if (PUNCTUATION.contains(current)) {
        valueBuffer += buffer.next();
        return makeToken(Punctuation, valueBuffer);
    }

    // Non-alpha operators
    switch(current) {
        case '=': case '+': case '-': case '^':
        case '*': case '/': case '>': case '<':
            valueBuffer += buffer.next();
            if (buffer.peek() == '=') valueBuffer += buffer.next();
            return makeToken(Operator, valueBuffer);
        case '!':
            valueBuffer += buffer.next();
            if (buffer.peek() != '=') return makeToken(Illegal, valueBuffer);
            valueBuffer += buffer.next();
            return makeToken(Operator, valueBuffer);
        default:
            // finally, if nothing clicks, return illegal
            return readIllegalToken(valueBuffer);
    }
}

Token Lexer::readIllegalToken(std::string &valueBuffer) {
    readWhile(valueBuffer, [](auto ch) {
        const auto result = isspace(static_cast<int>(ch));
        return !static_cast<bool>(result);
    });
    return makeToken(TokenType::Illegal, valueBuffer);
}

Token Lexer::readWordToken(std::string &valueBuffer) {
    readWhile(valueBuffer, [](auto ch) {
        const auto isAlphanum = std::isalnum(static_cast<int>(ch));
        return static_cast<bool>(isAlphanum) || ch == '_';
    });
    using enum TokenType;
    if (KEYWORDS.contains(valueBuffer)) {
        return makeToken(Keyword, valueBuffer);
    }
    if (OPERATORS.contains(valueBuffer)) {
        return makeToken(Operator, valueBuffer);
    }
    return makeToken(Identifier, valueBuffer);
}

Token Lexer::readNumberToken(std::string &valueBuffer) {
    const auto readDigits = [this](std::string &value) {
        readWhile(value, [](auto ch) {
            const auto isDigit = std::isdigit(static_cast<int>(ch));
            return static_cast<bool>(isDigit);
        });
    };
    readDigits(valueBuffer);
    if (buffer.peek() == '.') {
        valueBuffer += buffer.next();
        readDigits(valueBuffer);
    }
    return makeToken(TokenType::Number, valueBuffer);
}

Token Lexer::makeToken(TokenType type, std::string &valueBuffer) const {
    return {
        type, std::move(valueBuffer),
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

void Lexer::readWhile(std::string &valueBuffer, const std::function<bool(char)> &predicate) {
    while (!buffer.eof() && predicate(buffer.peek())) {
        const auto letter = buffer.next();
        valueBuffer += letter;
    }
}
