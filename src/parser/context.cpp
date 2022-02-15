#include "context.h"


namespace Cubiq::Parser {

    IdentifierInfo* GraphContext::find(const std::string& name) {
        if (auto it = identifiers.find(name); it != identifiers.end())
            return &it->second;
        return nullptr;
    }

    IdentifierInfo* GraphContext::create(std::string name, DataType type, Expression def) {
        return &identifiers.emplace(name, IdentifierInfo{type, def}).first->second;
    }

    IdentifierInfo* GraphContext::remove(const std::string& name) {
        if (auto it = identifiers.find(name); it != identifiers.end()) {
            IdentifierInfo* info = &it->second;
            identifiers.erase(it);
            return info;
        }
        return nullptr;
    }

}