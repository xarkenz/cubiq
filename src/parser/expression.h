#pragma once

#include <vector>

#include "defs.h"
#include "tokenizer.h"


namespace Cubiq::Parser {

    class GraphContext;
    enum struct DataType;

    class Expression {

    public:
        using Value = std::variant<Operation, Number, Symbol, Empty>;
    
    private:
        Value value;
        std::vector<Expression> children;
        DataType type;
    
    public:
        Expression();
        Expression(GraphContext& context, Value exprValue, std::vector<Expression> exprChildren);

        bool isOperation() const;
        bool isNumber() const;
        bool isSymbol() const;
        bool isEmpty() const;

        Operation getOperation() const;
        Number getNumber() const;
        Symbol getSymbol() const;

        const Value& getValue() const;
        DataType getType() const;
        const std::vector<Expression>& getChildren() const;

        void setValue(Value exprValue);
        void setType(DataType exprType);

        std::string toString() const;

    };

    bool operator==(const Expression& expr1, const Expression& expr2);
    bool operator!=(const Expression& expr1, const Expression& expr2);

}