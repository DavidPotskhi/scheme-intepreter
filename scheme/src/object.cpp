#include "scheme/object.h"
#include "error.h"

Ptr<Object> Object::Eval(Ptr<Object> ast, Ptr<Environemnt> env) {
    if (ast == nullptr) {
        throw RuntimeError("Empty list can not be evaluated");
    }
    return ast->Eval(env);
}
std::string Object::ToString(Ptr<Object> object) {
    if (object == nullptr) {
        return "()";
    }
    return object->ToString();
}
Ptr<Object> Symbol::Eval(Ptr<Environemnt> env) {
    return env->operator[](name_);
}
Ptr<Object> Environemnt::Eval(Ptr<Environemnt> env) {
    return shared_from_this();
}
std::string Environemnt::ToString() {
    std::string res;
    for (const auto& [symbol, binding] : bindings_) {
        res += symbol;
        res += " : ";
        res += Object::ToString(binding);
        res += '\n';
    }
    return res;
}
Ptr<Object> Environemnt::operator[](const std::string& symbol) {
    if (!bindings_.contains(symbol)) {
        throw RuntimeError("No defined entity in our current environment");
    }
    return bindings_[symbol];
}
void Environemnt::FullfillR5RS() {
    bindings_["+"] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        int res = 0;
        for (Ptr<Number> ptr : args) {
            res += ptr->GetValue();
        }
        return std::make_shared<Number>(res);
    });
    bindings_["-"] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        if (args.empty()) {
            throw RuntimeError("Too few arguments");
        }
        int res = args.front()->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            res -= args[i]->GetValue();
        }
        return std::make_shared<Number>(res);
    });
    bindings_["*"] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        int res = 1;
        for (Ptr<Number> ptr : args) {
            res *= ptr->GetValue();
        }
        return std::make_shared<Number>(res);
    });
    bindings_["/"] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        if (args.empty()) {
            throw RuntimeError("Too few arguments");
        }
        int res = args.front()->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            res /= args[i]->GetValue();
        }
        return std::make_shared<Number>(res);
    });
    bindings_[">"] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i - 1]->GetValue() <= args[i]->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    });
    bindings_["<"] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i - 1]->GetValue() >= args[i]->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    });
    bindings_[">="] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i - 1]->GetValue() < args[i]->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    });
    bindings_["<="] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i - 1]->GetValue() > args[i]->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    });
    bindings_["="] = std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i - 1]->GetValue() != args[i]->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    });
    bindings_["max"] =
        std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
            if (args.empty()) {
                throw RuntimeError("max should have at least 1 argument");
            }
            int maximum = INT_MIN;
            for (const Ptr<Number>& ptr : args) {
                maximum = std::max(maximum, ptr->GetValue());
            }
            return std::make_shared<Number>(maximum);
        });
    bindings_["min"] =
        std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
            if (args.empty()) {
                throw RuntimeError("min should have at least 1 argument");
            }
            int minimum = INT_MAX;
            for (const Ptr<Number>& ptr : args) {
                minimum = std::min(minimum, ptr->GetValue());
            }
            return std::make_shared<Number>(minimum);
        });
    bindings_["abs"] =
        std::make_shared<Procedure<Number>>([](const std::vector<Ptr<Number>>& args) {
            if (args.empty() || args.size() >= 2) {
                throw RuntimeError("abs should have 1 argument");
            }
            return std::make_shared<Number>(abs(args.front()->GetValue()));
        });
    bindings_["number?"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 1) {
                throw RuntimeError("number? should have exact 1 argument");
            }
            return std::make_shared<Boolean>(Is<Number>(args.front()));
        });
    bindings_["boolean?"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 1) {
                throw RuntimeError("number? should have exact 1 argument");
            }
            return std::make_shared<Boolean>(Is<Boolean>(args.front()));
        });
    bindings_["#t"] = std::make_shared<Boolean>(true);
    bindings_["#f"] = std::make_shared<Boolean>(false);
    bindings_["quote"] = std::make_shared<Syntax>(
        [](Ptr<Object> ast, Ptr<Environemnt> env) { return As<Cell>(ast)->GetFirst(); });
    bindings_["not"] = std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>> args) {
        if (args.size() != 1) {
            throw RuntimeError("not must have exact one argument");
        }
        if (Is<Boolean>(args.front())) {
            return std::make_shared<Boolean>(!As<Boolean>(args.front())->var_);
        }
        return std::make_shared<Boolean>(false);
    });
    bindings_["and"] =
        std::make_shared<Syntax>([](Ptr<Object> ast, Ptr<Environemnt> env) -> Ptr<Object> {
            std::vector<Ptr<Object>> args = CollectArguments(ast);
            for (const Ptr<Object>& ptr : args) {
                Ptr<Object> arg = Object::Eval(ptr, env);
                if (Is<Boolean>(arg) && !As<Boolean>(arg)->var_) {
                    return std::make_shared<Boolean>(false);
                }
            }
            if (args.empty()) {
                return std::make_shared<Boolean>(true);
            }
            return Object::Eval(args.back(), env);
        });
    bindings_["or"] =
        std::make_shared<Syntax>([](Ptr<Object> ast, Ptr<Environemnt> env) -> Ptr<Object> {
            std::vector<Ptr<Object>> args = CollectArguments(ast);
            for (const Ptr<Object>& ptr : args) {
                Ptr<Object> arg = Object::Eval(ptr, env);
                if (Is<Boolean>(arg) && As<Boolean>(arg)->var_) {
                    return std::make_shared<Boolean>(true);
                }
                if (!Is<Boolean>(arg)) {
                    return Object::Eval(arg, env);
                }
            }
            return std::make_shared<Boolean>(false);
        });
    bindings_["pair?"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 1) {
                throw RuntimeError("pair? should have at least one argument");
            }
            Ptr<Object> arg = args.front();
            if (!Is<Cell>(arg)) {
                return std::make_shared<Boolean>(false);
            }
            return std::make_shared<Boolean>(true);
        });
    bindings_["null?"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 1) {
                throw RuntimeError("null? should have at least one argument");
            }
            Ptr<Object> arg = args.front();
            if (arg != nullptr) {
                return std::make_shared<Boolean>(false);
            }
            return std::make_shared<Boolean>(true);
        });
    bindings_["list?"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 1) {
                throw RuntimeError("list? should have at least one argument");
            }
            Ptr<Object> arg = args.front();

            while (Is<Cell>(arg)) {
                arg = As<Cell>(arg)->GetSecond();
            }
            return std::make_shared<Boolean>(arg == nullptr);
        });
    bindings_["cons"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 2) {
                throw RuntimeError("cons must have at least 2 arguments");
            }
            return std::make_shared<Cell>(args.front(), args.back());
        });
    bindings_["car"] = std::make_shared<Procedure<Cell>>([](const std::vector<Ptr<Cell>>& args) {
        if (args.size() != 1) {
            throw RuntimeError("cons must have at least 2 arguments");
        }
        return args.front()->GetFirst();
    });
    bindings_["cdr"] = std::make_shared<Procedure<Cell>>([](const std::vector<Ptr<Cell>>& args) {
        if (args.size() != 1) {
            throw RuntimeError("cons must have at least 2 arguments");
        }
        return args.front()->GetSecond();
    });
    bindings_["list"] = std::make_shared<Procedure<Object>> ([](const std::vector<Ptr<Object>>& args) -> Ptr<Object> {
        if (args.empty()) {
            return nullptr;
        }
        Ptr<Cell> next = std::make_shared<Cell>(nullptr, nullptr);
        Ptr<Cell> cur;
        Ptr<Cell> root = next;
        for (const Ptr<Object>& ptr : args) {
          cur = next;
          cur->GetFirst() = ptr;
          next =  std::make_shared<Cell>(nullptr, nullptr);
          cur->GetSecond() = next;
        }
        cur->GetSecond() = nullptr;
        return root;
    });
    bindings_["list-ref"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) {
            if (args.size() != 2) {
                throw RuntimeError("list-ref must have exactly 2 arguments");
            }
            if (!Is<Number>(args.back())) {
                throw RuntimeError("Index in list-ref must be integer");
            }
            if (As<Number>(args.back())->GetValue() < 0) {
                throw RuntimeError("Index in list-ref must be positive integer");
            }
            std::vector<Ptr<Object>> array = CollectArguments(args.front());
            if (As<Number>(args.back())->GetValue() >= array.size()) {
                throw RuntimeError("Index out of bound in list-ref");
            }
            return array[As<Number>(args.back())->GetValue()];
        });
    bindings_["list-tail"] =
        std::make_shared<Procedure<Object>>([](const std::vector<Ptr<Object>>& args) -> Ptr<Object> {
            if (args.size() != 2) {
                throw RuntimeError("list-ref must have exactly 2 arguments");
            }
            if (!Is<Number>(args.back())) {
                throw RuntimeError("Index in list-ref must be integer");
            }
            if (As<Number>(args.back())->GetValue() < 0) {
                throw RuntimeError("Index in list-ref must be positive integer");
            }
          std::vector<Ptr<Object>> array = CollectArguments(args.front());
          if (As<Number>(args.back())->GetValue() - array.size() == 0) {
              return nullptr;
          }
          if (As<Number>(args.back())->GetValue() >= array.size()) {
                throw RuntimeError("Index out of bound in list-ref");
            }
            Ptr<Cell> root = nullptr;
            Ptr<Cell> cur = As<Cell>(args.front());
            for (size_t i = 0; i <= As<Number>(args.back())->GetValue(); ++i) {
                root = cur;
                cur = As<Cell>(cur->GetSecond());
            }
            return root;
        });
}
std::shared_ptr<Object> Cell::Eval(Ptr<Environemnt> env) {
    Ptr<Object> f = Object::Eval(GetFirst(), env);
    return As<Callable>(f)->Call(GetSecond(), env);
}
std::string Cell::ToString() {
    std::string res = "(";
    Ptr<Object> cur = shared_from_this();
    while (cur != nullptr) {
        if (Is<Cell>(cur)) {
            res += Object::ToString(As<Cell>(cur)->GetFirst());
            cur = As<Cell>(cur)->GetSecond();
        } else {
            res += ". ";
            res += Object::ToString(cur);
            cur = nullptr;
        }
        res += ' ';
    }
    res.pop_back();
    res += ')';
    return res;
}

std::string Boolean::ToString() {
    if (var_) {
        return "#t";
    }
    return "#f";
}
std::vector<Ptr<Object>> CollectArguments(Ptr<Object> ast) {
    std::vector<Ptr<Object>> res;
    Ptr<Object> cur = ast;
    while (cur != nullptr) {
        if (Is<Cell>(cur)) {
            res.push_back(As<Cell>(cur)->GetFirst());
            cur = As<Cell>(cur)->GetSecond();
        } else {
            res.push_back(cur);
            cur = nullptr;
        }
    }
    return res;
}
Ptr<Object> Syntax::Eval(Ptr<Environemnt> env) {
    return shared_from_this();
}
std::string Syntax::ToString() {
    return "BuiltIn Syntax";
}
Ptr<Object> Syntax::Call(Ptr<Object> ast, Ptr<Environemnt> env) {
    return function_(ast, env);
}
