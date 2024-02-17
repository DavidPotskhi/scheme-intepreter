#pragma once

#include <memory>
#include "error.h"
#include <unordered_map>
#include <functional>

template <typename T>
using Ptr = std::shared_ptr<T>;

class Interpreter;

class Environemnt;

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual Ptr<Object> Eval(Ptr<Environemnt> env) = 0;
    virtual std::string ToString() = 0;
    static Ptr<Object> Eval(Ptr<Object> ast, Ptr<Environemnt> env);
    static std::string ToString(Ptr<Object> object);
    virtual ~Object() = default;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

class Environemnt : public Object {
public:
    Ptr<Object> Eval(Ptr<Environemnt> env) override;
    std::string ToString() override;
    Ptr<Object> operator[](const std::string& symbol);
    void FullfillR5RS();
    ~Environemnt() override = default;

private:
    std::unordered_map<std::string, Ptr<Object>> bindings_;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }
    int GetValue() const {
        return value_;
    }

    Ptr<Object> Eval(Ptr<Environemnt> env) override {
        return shared_from_this();
    }

    std::string ToString() override {
        return std::to_string(value_);
    }

private:
    int value_;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

class Symbol : public Object {
public:
    Symbol(const std::string& name) : name_(name) {
    }
    const std::string& GetName() const {
        return name_;
    }

    Ptr<Object> Eval(Ptr<Environemnt> env) override;

    std::string ToString() override {
        return name_;
    }

private:
    std::string name_;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

class Cell : public Object {
public:
    Cell(const Ptr<Object>& first, const Ptr<Object>& second) : first_(first), second_(second) {
    }
    Ptr<Object>& GetFirst() {
        return first_;
    }
    Ptr<Object>& GetSecond() {
        return second_;
    }
    std::string ToString() override;
    ~Cell() override = default;

    std::shared_ptr<Object> Eval(Ptr<Environemnt> env) override;

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class Callable : public Object {
public:
    virtual Ptr<Object> Call(Ptr<Object> ast, Ptr<Environemnt> env) = 0;
    virtual ~Callable() = default;
};

template <typename T>
class Procedure : public Callable {
public:
    Procedure(std::function<Ptr<Object>(const std::vector<Ptr<T>>&)> function)
        : function_(function) {
    }
    Ptr<Object> Eval(Ptr<Environemnt> env) override;
    std::string ToString() override;
    ~Procedure() override = default;

public:
    Ptr<Object> Call(Ptr<Object> ast, Ptr<Environemnt> env) override;

private:
    std::function<Ptr<Object>(const std::vector<Ptr<T>>&)> function_;
};
std::vector<Ptr<Object>> CollectArguments(Ptr<Object> ast);
template <typename T>
Ptr<Object> Procedure<T>::Call(Ptr<Object> ast, Ptr<Environemnt> env) {
    std::vector<Ptr<Object>> args = CollectArguments(ast);
    std::vector<Ptr<T>> evaluated_args;
    for (Ptr<Object> ptr : args) {
        evaluated_args.push_back(As<T>(Object::Eval(ptr, env)));
    }
    return function_(evaluated_args);
}
template <typename T>
Ptr<Object> Procedure<T>::Eval(Ptr<Environemnt> env) {
    return shared_from_this();
}

template <typename T>
std::string Procedure<T>::ToString() {
    return "BuiltIn Procedure";
}

class Syntax : public Callable {
public:
    Syntax(std::function<Ptr<Object>(Ptr<Object>, Ptr<Environemnt>)> function)
        : function_(function) {
    }
    Ptr<Object> Eval(Ptr<Environemnt> env) override;
    std::string ToString() override;
    ~Syntax() override = default;
    Ptr<Object> Call(Ptr<Object> ast, Ptr<Environemnt> env) override;

private:
    std::function<Ptr<Object>(Ptr<Object>, Ptr<Environemnt>)> function_;
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj);

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj);

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

class PairIndicator : public Object {
public:
    Ptr<Object> Eval(Ptr<Environemnt> env) override {
        return std::make_shared<Number>(1);
    }
    std::string ToString() override {
        return std::string();
    }
    ~PairIndicator() override = default;
};

class CloseBracketIndicator : public Object {
public:
    Ptr<Object> Eval(Ptr<Environemnt> env) override {
        return std::make_shared<Number>(1);
    }
    std::string ToString() override {
        return std::string();
    }
    ~CloseBracketIndicator() override = default;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

class Boolean : public Object {
public:
    Boolean(bool var) : var_(var) {
    }
    Ptr<Object> Eval(Ptr<Environemnt> env) override {
        return shared_from_this();
    }
    std::string ToString() override;
    bool var_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    if constexpr (std::is_same_v<T, Object>) {
        return obj;
    }
    std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(obj);
    if (ptr == nullptr) {
        throw RuntimeError("Types don't match");
    }
    return ptr;
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
}
