#pragma once

#include <functional>
#include <string>
#include <variant>


namespace Cubiq::Parser {

    using CharStream = std::function<int()>;

    using Number = double;

    struct Symbol {
        std::string name;
    };

    bool operator==(const Symbol& s1, const Symbol& s2);
    bool operator!=(const Symbol& s1, const Symbol& s2);

    struct Empty {
    };
    
    bool operator==(const Empty&, const Empty&);
    bool operator!=(const Empty&, const Empty&);


    class Token {
        
    public:
        using Value = std::variant<Number, Symbol, Empty>;
    
    private:
        Value value;
    
    public:
        Token(Value val);

        bool isNumber() const;
        bool isSymbol() const;
        bool isEmpty() const;

        const Value& getValue() const;

        Number getNumber() const;
        const Symbol& getSymbol() const;

        bool hasValue(Value value) const;

        std::string toString() const;

    };


    class TokenIterator {

    private:
        CharStream& charStream;
        Token current;
    
    public:
        TokenIterator(CharStream& stream);

        const Token& operator*() const;
        const Token* operator->() const;
        TokenIterator& operator++();
        explicit operator bool() const;
        
    };

}