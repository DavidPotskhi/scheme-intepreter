#include <scheme/parser.h>
#include <error.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Read reached the end, but not Close Bracket found");
    }
    auto token = tokenizer->GetToken();
    tokenizer->Next();
    if (token == Token{BracketToken::OPEN}) {
        return ReadList(tokenizer);
    }
    if (token == Token{BracketToken::CLOSE}) {
        throw SyntaxError("No matching open bracket");
    }
    if (auto* ptr = get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(ptr->value);
    }
    if (auto* ptr = get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(ptr->name);
    }
    if (auto* ptr = get_if<DotToken>(&token)) {
        throw SyntaxError("Dot should be before last element of list");
    }
    if (auto* ptr = get_if<QuoteToken>(&token)) {
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"),
                                      std::make_shared<Cell>(Read(tokenizer), nullptr));
    }
    throw SyntaxError("exception in Read");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Cell> cell = std::make_shared<Cell>(nullptr, nullptr);
    if (tokenizer->IsEnd()) {
        throw SyntaxError("ReadList reached the end, but not Close Bracket found");
    }
    Token token = tokenizer->GetToken();
    if (auto* ptr = std::get_if<BracketToken>(&token)) {
        if (*ptr == BracketToken::CLOSE) {
            tokenizer->Next();
            return nullptr;
        }
    }
    Ptr<Object> first = Read(tokenizer);
    if (tokenizer->IsEnd()) {
        throw SyntaxError("ReadList reached the end, but not Close Bracket found");
    }
    token = tokenizer->GetToken();
    if (auto* ptr = get_if<DotToken>(&token)) {
        tokenizer->Next();
        cell->GetSecond() = Read(tokenizer);
        if (tokenizer->IsEnd()) {
            throw SyntaxError("ReadList reached the end, but not Close Bracket found");
        }
        token = tokenizer->GetToken();
        auto* p = get_if<BracketToken>(&token);
        if (p == nullptr || *p != BracketToken::CLOSE) {
            throw SyntaxError("No closing bracket in pair");
        }
        tokenizer->Next();
        cell->GetFirst() = first;
        return cell;
    }
    cell->GetFirst() = first;
    cell->GetSecond() = ReadList(tokenizer);
    return cell;
}
