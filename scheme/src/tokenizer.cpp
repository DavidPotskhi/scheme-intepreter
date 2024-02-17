#include <scheme/tokenizer.h>
#include <string>

void Tokenizer::Next() {
    if (s_->eof()) {
        current_token_.reset();
        return;
    }
    while (std::isblank(s_->peek()) || s_->peek() == '\n' || s_->peek() == '\r') {
        s_->get();
    }
    if (s_->eof()) {
        current_token_.reset();
        return;
    }
    if (s_->peek() == '(') {
        current_token_ = BracketToken::OPEN;
        s_->get();
    } else if (s_->peek() == ')') {
        current_token_ = BracketToken::CLOSE;
        s_->get();
    } else if (s_->peek() == '\'') {
        current_token_ = QuoteToken();
        s_->get();
    } else if (s_->peek() == '.') {
        current_token_ = DotToken();
        s_->get();
    } else if (starts_from(s_->peek(), first_might_begin_)) {
        SymbolToken token;
        while (starts_from(s_->peek(), word_contains)) {
            token.name += s_->get();
        }
        current_token_ = token;
    } else if (s_->peek() == '+' || s_->peek() == '-') {
        char sign = s_->get();
        if (std::isblank(s_->peek()) || s_->eof() || !std::isdigit(s_->peek())
            || s_->peek() == '\n' || s_->peek() == '\r') {
            SymbolToken token;
            token.name += sign;
            current_token_ = token;
        }
        if (std::isdigit(s_->peek())) {
            std::string integer;
            integer += sign;
            while (std::isdigit(s_->peek())) {
                integer += s_->get();
            }
            ConstantToken token;
            token.value = std::stoi(integer);
            current_token_ = token;
        }
    } else if (std::isdigit(s_->peek())) {
        std::string integer;
        while (std::isdigit(s_->peek())) {
            integer += s_->get();
        }
        ConstantToken token;
        token.value = std::stoi(integer);
        current_token_ = token;
    }
}
