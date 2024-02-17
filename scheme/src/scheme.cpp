#include "scheme/scheme.h"
#include "scheme/error.h"
#include "scheme/tokenizer.h"
#include "scheme/parser.h"
#include <sstream>

std::string Interpreter::Run(const std::string& s) {
    std::stringstream ss(s);
    Tokenizer t(&ss);
    auto node = Read(&t);
    auto result = Object::Eval(node, global_scope_);
    return Object::ToString(result);
}
