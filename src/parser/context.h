#pragma once

#include "expression.h"


namespace Cubiq::Parser {

    struct IdentifierInfo {
        DataType type;
        Expression def;
    };


    class GraphContext {

    private:
        std::unordered_map<std::string, IdentifierInfo> identifiers;

    public:
        IdentifierInfo* find(const std::string& name);
        IdentifierInfo* create(std::string name, DataType type, Expression def);
        IdentifierInfo* remove(const std::string& name);

    };

}