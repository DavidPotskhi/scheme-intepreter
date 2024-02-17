#pragma once

#include <string>
#include <map>
#include <functional>
#include "object.h"

class Object;
class Boolean;
class Number;



class Interpreter {
public:
    Interpreter() {
        global_scope_ = std::make_shared<Environemnt>();
        global_scope_->FullfillR5RS();
    }

    std::string Run(const std::string& s);

private:
    Ptr<Environemnt> global_scope_;
};
