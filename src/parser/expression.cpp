#include "expression.h"
#include "context.h"


namespace Cubiq::Parser {

    Expression::Expression() : value(Empty()), type(DataType::NOTHING) {
    }

    Expression::Expression(GraphContext& context, Expression::Value v, std::vector<Expression> c) : value(v), children(c) {
        if (isEmpty()) {
            type = DataType::NOTHING;
        } else if (isOperation()) {
            type = DataType::UNRESOLVED;
        } else if (isNumber()) {
            type = DataType::NUMBER;
        } else if (isSymbol()) {
            std::string name = getSymbol().name;
            if (IdentifierInfo* info = context.find(name)) {
                type = info->type;
            } else {
                type = DataType::UNRESOLVED;
            }
        }
    }


    bool Expression::isOperation() const {
        return std::holds_alternative<Operation>(value);
    }

    bool Expression::isNumber() const {
        return std::holds_alternative<Number>(value);
    }

    bool Expression::isSymbol() const {
        return std::holds_alternative<Symbol>(value);
    }

    bool Expression::isEmpty() const {
        return std::holds_alternative<Empty>(value);
    }


    Operation Expression::getOperation() const {
        return std::get<Operation>(value);
    }

    Number Expression::getNumber() const {
        return std::get<Number>(value);
    }

    Symbol Expression::getSymbol() const {
        return std::get<Symbol>(value);
    }


    const Expression::Value& Expression::getValue() const {
        return value;
    }

    DataType Expression::getType() const {
        return type;
    }

    const std::vector<Expression>& Expression::getChildren() const {
        return children;
    }


    void Expression::setValue(Expression::Value exprValue) {
        value = std::move(exprValue);
    }

    void Expression::setType(DataType exprType) {
        type = exprType;
    }


    std::string Expression::toString() const {
        if (isEmpty()) {
            return "(empty)";
        } else if (isNumber()) {
            return std::to_string(getNumber());
        } else if (isSymbol()) {
            return "$" + getSymbol().name;
        } else if (isOperation()) {
            switch (getOperation()) {
                case Operation::POS:
                    return "(+" + children[0].toString() + ")";
                case Operation::NEG:
                    return "(-" + children[0].toString() + ")";
                case Operation::B_NOT:
                    return "(~" + children[0].toString() + ")";
                case Operation::L_NOT:
                    return "(!" + children[0].toString() + ")";
                case Operation::FACT:
                    return "(" + children[0].toString() + "!)";
                case Operation::ADD:
                    return "(" + children[0].toString() + " + " + children[1].toString() + ")";
                case Operation::SUB:
                    return "(" + children[0].toString() + " - " + children[1].toString() + ")";
                case Operation::MUL:
                    return "(" + children[0].toString() + " * " + children[1].toString() + ")";
                case Operation::DIV:
                    return "(" + children[0].toString() + " / " + children[1].toString() + ")";
                case Operation::MOD:
                    return "(" + children[0].toString() + " % " + children[1].toString() + ")";
                case Operation::EXP:
                    return "(" + children[0].toString() + " ** " + children[1].toString() + ")";
                case Operation::SQRT:
                    return "(" + children[0].toString() + "-root " + children[1].toString() + ")";
                case Operation::SIN:
                    return "(sin " + children[0].toString() + ")";
                case Operation::COS:
                    return "(cos " + children[0].toString() + ")";
                case Operation::TAN:
                    return "(tan " + children[0].toString() + ")";
                case Operation::SEC:
                    return "(sec " + children[0].toString() + ")";
                case Operation::CSC:
                    return "(csc " + children[0].toString() + ")";
                case Operation::COT:
                    return "(cot " + children[0].toString() + ")";
                case Operation::B_AND:
                    return "(" + children[0].toString() + " & " + children[1].toString() + ")";
                case Operation::B_OR:
                    return "(" + children[0].toString() + " | " + children[1].toString() + ")";
                case Operation::B_XOR:
                    return "(" + children[0].toString() + " ^ " + children[1].toString() + ")";
                case Operation::B_LSHIFT:
                    return "(" + children[0].toString() + " << " + children[1].toString() + ")";
                case Operation::B_RSHIFT:
                    return "(" + children[0].toString() + " >> " + children[1].toString() + ")";
                case Operation::EQ:
                    return "(" + children[0].toString() + " == " + children[1].toString() + ")";
                case Operation::NEQ:
                    return "(" + children[0].toString() + " != " + children[1].toString() + ")";
                case Operation::LT:
                    return "(" + children[0].toString() + " < " + children[1].toString() + ")";
                case Operation::GT:
                    return "(" + children[0].toString() + " > " + children[1].toString() + ")";
                case Operation::LTEQ:
                    return "(" + children[0].toString() + " <= " + children[1].toString() + ")";
                case Operation::GTEQ:
                    return "(" + children[0].toString() + " >= " + children[1].toString() + ")";
                case Operation::COMMA:
                    return "(" + children[0].toString() + ", " + children[1].toString() + ")";
                case Operation::L_AND:
                    return "(" + children[0].toString() + " && " + children[1].toString() + ")";
                case Operation::L_OR:
                    return "(" + children[0].toString() + " || " + children[1].toString() + ")";
                case Operation::L_XOR:
                    return "(" + children[0].toString() + " ^^ " + children[1].toString() + ")";
                case Operation::INDEX:
                    return "(" + children[0].toString() + "[" + children[1].toString() + "])";
                case Operation::CALL: {
                    std::string str = children[0].toString() + "(";
                    const char* separator = "";
                    for (size_t i = 1; i < children.size(); ++i) {
                        str += separator + children[i].toString();
                        separator = ", ";
                    }
                    return str + ")";
                }
                case Operation::POINT: {
                    std::string str = "(";
                    const char* separator = "";
                    for (size_t i = 0; i < children.size(); ++i) {
                        str += separator + children[i].toString();
                        separator = ", ";
                    }
                    return str + ")";
                }
                case Operation::ARRAY: {
                    std::string str = "[";
                    const char* separator = "";
                    for (size_t i = 0; i < children.size(); ++i) {
                        str += separator + children[i].toString();
                        separator = ", ";
                    }
                    return str + "]";
                }
                case Operation::BRANCH: {
                    std::string str = "{";
                    const char* separator = "";
                    for (size_t i = 0; i < children.size(); ++i) {
                        str += separator + children[i].toString();
                        separator = i % 2 == 0 ? ": " : ", ";
                    }
                    return str + "}";
                }
                default:
                    return "(unknown op)";
            }
        } else {
            return "(unknown)";
        }
    }


    bool operator==(const Expression& expr1, const Expression& expr2) {
        return expr1.getValue() == expr2.getValue()
            && expr1.getChildren() == expr2.getChildren()
            && expr1.getType() == expr2.getType();
    }

    bool operator!=(const Expression& expr1, const Expression& expr2) {
        return !(expr1 == expr2);
    }

}