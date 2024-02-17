#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <array>

struct SymbolToken {
    std::string name;
    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : s_(in), current_token_() {
        Next();
    }

    bool IsEnd() {
        return !current_token_.has_value();
    }

    void Next();

    Token GetToken() {
        return current_token_.value();
    }

private:
    template <size_t N>
    static bool starts_from(char a, const std::array<char, N>& might_begin) {
        for (char el : might_begin) {
            if (a == el) {
                return true;
            }
        }
        return false;
    }
private:
    std::istream* s_;
    std::optional<Token> current_token_;
    constexpr static std::array first_might_begin_ = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                                                      'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                                                      'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                                                      'y', 'z',
                                                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                                      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                                      'Y', 'Z', '<', '=', '>', '*', '/', '#'};
    constexpr static std::array word_contains = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                                                 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                                                 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                                                 'y', 'z',
                                                 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                                 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                                 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                                 'Y', 'Z', '<', '=', '>', '*', '/', '#',
                                                 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                 '?', '!', '-'};
};