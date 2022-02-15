#include "interpreter.h"

#include <stack>
#include <iostream>


namespace Cubiq::Parser {

    namespace {

        enum struct OperatorPrecedence {
            ARGUMENT,       // \s{y}
            POSTFIX,        // x[...]  x(...)  x!
            PREFIX,         // +x  -x  ~x  !x
            EXPONENTATIVE,  // x**y
            MULTIPLICATIVE, // x*y  x/y  x%y
            ADDITIVE,       // x+y  x-y
            SHIFT,          // x<<y  x>>y
            INEQUALITY,     // x<y  x>y  x<=y  x>=y
            EQUALITY,       // x=y  x!=y
            BITWISE_AND,    // x&y
            BITWISE_XOR,    // x^y
            BITWISE_OR,     // x|y
            LOGICAL_AND,    // x&&y
            LOGICAL_XOR,    // x^^y
            LOGICAL_OR,     // x||y
            COMMA,          // x,y
            CONTAINER,      // (...)  [...]  {...}
        };


        enum struct OperatorAssociativity {
            LEFT_TO_RIGHT,
            RIGHT_TO_LEFT,
        };


        struct OperatorInfo {
            Operation operation;
            OperatorPrecedence precedence;
            OperatorAssociativity associativity;
            int numOperands;

            OperatorInfo(Operation operation) : operation(operation) {
                associativity = OperatorAssociativity::LEFT_TO_RIGHT;
                numOperands = 2;

                switch (operation) {
                    case Operation::DIV:
                    case Operation::SQRT:
                        precedence = OperatorPrecedence::ARGUMENT;
                        break;
                    
                    case Operation::INDEX:
                    case Operation::CALL:
                    case Operation::FACT:
                        precedence = OperatorPrecedence::POSTFIX;
                        numOperands = 1;
                        break;

                    case Operation::POS:
                    case Operation::NEG:
                    case Operation::B_NOT:
                    case Operation::L_NOT:
                        precedence = OperatorPrecedence::PREFIX;
                        associativity = OperatorAssociativity::RIGHT_TO_LEFT;
                        numOperands = 1;
                        break;

                    case Operation::EXP:
                        precedence = OperatorPrecedence::EXPONENTATIVE;
                        break;

                    case Operation::MUL:
                    case Operation::MOD:
                        precedence = OperatorPrecedence::MULTIPLICATIVE;
                        break;

                    case Operation::ADD:
                    case Operation::SUB:
                        precedence = OperatorPrecedence::ADDITIVE;
                        break;

                    case Operation::B_LSHIFT:
                    case Operation::B_RSHIFT:
                        precedence = OperatorPrecedence::SHIFT;
                        break;

                    case Operation::LT:
                    case Operation::GT:
                    case Operation::LTEQ:
                    case Operation::GTEQ:
                        precedence = OperatorPrecedence::INEQUALITY;
                        break;

                    case Operation::EQ:
                    case Operation::NEQ:
                        precedence = OperatorPrecedence::EQUALITY;
                        break;

                    case Operation::B_AND:
                        precedence = OperatorPrecedence::BITWISE_AND;
                        break;

                    case Operation::B_XOR:
                        precedence = OperatorPrecedence::BITWISE_XOR;
                        break;

                    case Operation::B_OR:
                        precedence = OperatorPrecedence::BITWISE_OR;
                        break;

                    case Operation::L_AND:
                        precedence = OperatorPrecedence::LOGICAL_AND;
                        break;

                    case Operation::L_XOR:
                        precedence = OperatorPrecedence::LOGICAL_XOR;
                        break;

                    case Operation::L_OR:
                        precedence = OperatorPrecedence::LOGICAL_OR;
                        break;

                    case Operation::COMMA:
                        precedence = OperatorPrecedence::COMMA;
                        numOperands = 1;
                        break;

                    case Operation::POINT:
                    case Operation::ARRAY:
                    case Operation::BRANCH:
                        precedence = OperatorPrecedence::CONTAINER;
                        numOperands = operation == Operation::POINT ? 2 : 0;
                        break;
                }
            }
        };


        OperatorInfo getOperatorInfo(std::string symbol, bool prefix) {
            if (symbol == "+")
                return prefix ? OperatorInfo(Operation::POS) : OperatorInfo(Operation::ADD);
            if (symbol == "-")
                return prefix ? OperatorInfo(Operation::NEG) : OperatorInfo(Operation::SUB);
            if (symbol == "\\cdot")
                return OperatorInfo(Operation::MUL);
            if (symbol == "\\frac")
                return OperatorInfo(Operation::DIV);
            if (symbol == "%")
                return OperatorInfo(Operation::MOD);
            if (symbol == "^")
                return OperatorInfo(Operation::EXP);
            if (symbol == "\\sqrt")
                return OperatorInfo(Operation::SQRT);
            if (symbol == "!")
                return OperatorInfo(Operation::FACT);
            if (symbol == "\\lnot" || symbol == "\\neg")
                return OperatorInfo(Operation::L_NOT);
            if (symbol == "\\land" || symbol == "\\wedge")
                return OperatorInfo(Operation::L_AND);
            if (symbol == "\\lor" || symbol == "\\vee")
                return OperatorInfo(Operation::L_OR);
            if (symbol == "\\lxor" || symbol == "\\veebar")
                return OperatorInfo(Operation::L_XOR);
            if (symbol == "\\sim")
                return OperatorInfo(Operation::B_NOT);
            if (symbol == "\\band")
                return OperatorInfo(Operation::B_AND);
            if (symbol == "\\bor")
                return OperatorInfo(Operation::B_OR);
            if (symbol == "\\bxor")
                return OperatorInfo(Operation::B_XOR);
            if (symbol == "\\ll")
                return OperatorInfo(Operation::B_LSHIFT);
            if (symbol == "\\gg")
                return OperatorInfo(Operation::B_RSHIFT);
            if (symbol == "\\cdot")
                return OperatorInfo(Operation::MUL);
            if (symbol == "=")
                return OperatorInfo(Operation::EQ);
            if (symbol == "\\ne" || symbol == "\\neq")
                return OperatorInfo(Operation::NEQ);
            if (symbol == "<")
                return OperatorInfo(Operation::LT);
            if (symbol == ">")
                return OperatorInfo(Operation::GT);
            if (symbol == "\\leq")
                return OperatorInfo(Operation::LTEQ);
            if (symbol == "\\geq")
                return OperatorInfo(Operation::GTEQ);
            if (symbol == ",")
                return OperatorInfo(Operation::COMMA);
            if (symbol == "\\left(")
                return OperatorInfo(Operation::CALL);
            if (symbol == "\\left[")
                return OperatorInfo(Operation::INDEX);
            if (symbol == "\\left{")
                return OperatorInfo(Operation::BRANCH);
            else
                throw Error{ErrorType::UNEXPECTED, symbol};
        }


        bool isExpressionEnd(const Token& t, bool allowComma) {
            if (t.isSymbol()) {
                std::string s = t.getSymbol().name;
                if (s == ",")
                    return !allowComma;
                if (s == "}" || s == "]" || s == ";" || s == ":" || s == "\\right)" || s == "\\right]" || s == "\\right}")
                    return true;
                return false;
            }
            return t.isEmpty();
        }


        bool isEvaluatedBefore(const OperatorInfo& l, const OperatorInfo& r) {
            return l.associativity == OperatorAssociativity::LEFT_TO_RIGHT ? l.precedence <= r.precedence : l.precedence < r.precedence;
        }

        bool isValidIdentifier(const std::string& s) {
            if (s.length() == 1) {
                return std::isalpha(s[0]);
            }
            // TODO: better checking
            return s == "\\pi" || s == "\\theta" || s == "\\omega";
        }


        void popOperator(std::stack<OperatorInfo>& operatorStack, std::stack<Expression>& operandStack, GraphContext& context) {
            if (operandStack.size() < operatorStack.top().numOperands) {
                throw Error{ErrorType::UNKNOWN_ERROR, ""};
            }

            std::vector<Expression> operands;
            operands.resize(operatorStack.top().numOperands);

            for (int i = operatorStack.top().numOperands - 1; i >= 0; --i) {
                operands[i] = std::move(operandStack.top());
                operandStack.pop();
            }

            operandStack.emplace(context, operatorStack.top().operation, operands);

            operatorStack.pop();
        }


        Expression parseExpression(GraphContext& context, TokenIterator& it, bool allowEmpty, bool allowComma = false) {
            std::stack<Expression> operandStack;
            std::stack<OperatorInfo> operatorStack;

            bool expectingOperand = true;

            for (; !isExpressionEnd(*it, allowComma); ++it) {

                if (it->isSymbol() && !isValidIdentifier(it->getSymbol().name)) {
                    std::string sym = it->getSymbol().name;

                    // Expression grouping with {}
                    if (expectingOperand && sym == "{") {
                        ++it;
                        operandStack.push(parseExpression(context, it, false));
                        if (!it->isSymbol() || it->getSymbol().name != "}")
                            throw Error{ErrorType::MISSING, "}"};
                        expectingOperand = false;
                        continue;
                    }

                    OperatorInfo oi = getOperatorInfo(sym, expectingOperand);

                    // If an operand is expected, parse parentheses as expression grouping or point instead of function call
                    if (expectingOperand && oi.operation == Operation::CALL) {
                        ++it;
                        operandStack.push(parseExpression(context, it, false));
                        if (!it->isSymbol() || it->getSymbol().name != "\\right)") {
                            if (it->isSymbol() && it->getSymbol().name == ",") {
                                // Parentheses represent point, not grouping
                                operandStack.push(parseExpression(context, it, false));
                                if (!it->isSymbol() || it->getSymbol().name != "\\right)")
                                    throw Error{ErrorType::MISSING, "\\right)"};
                                operatorStack.push(OperatorInfo(Operation::POINT));
                            } else throw Error{ErrorType::MISSING, "\\right)"};
                        }
                        expectingOperand = false;
                        continue;
                    }

                    // If an operand is expected, parse square brackets as array instead of index
                    if (expectingOperand && oi.operation == Operation::INDEX) {
                        oi = OperatorInfo(Operation::ARRAY);
                        ++it;
                        if (!it->isSymbol() || it->getSymbol().name != "\\right]") {
                            while (true) {
                                Expression item = parseExpression(context, it, false);
                                operandStack.push(item);
                                ++oi.numOperands;
                                if (it->isSymbol() && it->getSymbol().name == "\\right]") break;
                                if (!it->isSymbol() || it->getSymbol().name != ",")
                                    throw Error{ErrorType::MISSING, "\\right]"};
                                ++it;
                            }
                        }
                        operatorStack.push(oi);
                        expectingOperand = false;
                        continue;
                    }

                    // TODO: parse piecewise (branch)

                    if (expectingOperand && oi.operation == Operation::DIV) {
                        ++it;
                        if (!it->isSymbol() || it->getSymbol().name != "{")
                            throw Error{ErrorType::MISSING, "{"};
                        ++it;
                        operandStack.push(parseExpression(context, it, true));
                        if (!it->isSymbol() || it->getSymbol().name != "}")
                            throw Error{ErrorType::MISSING, "}"};
                        ++it;
                        if (!it->isSymbol() || it->getSymbol().name != "{")
                            throw Error{ErrorType::MISSING, "{"};
                        ++it;
                        operandStack.push(parseExpression(context, it, true));
                        if (!it->isSymbol() || it->getSymbol().name != "}")
                            throw Error{ErrorType::MISSING, "}"};
                        operatorStack.push(oi);
                        expectingOperand = false;
                        continue;
                    }

                    if (expectingOperand && oi.operation == Operation::SQRT) {
                        ++it;
                        if (it->isSymbol() && it->getSymbol().name == "[") {
                            // Has n root specified
                            ++it;
                            operandStack.push(parseExpression(context, it, true));
                            if (!it->isSymbol() || it->getSymbol().name != "]")
                                throw Error{ErrorType::MISSING, "]"};
                            ++it;
                        } else {
                            // Defaults to square root
                            operandStack.emplace(context, 2, std::vector<Expression>());
                        }
                        if (!it->isSymbol() || it->getSymbol().name != "{")
                            throw Error{ErrorType::MISSING, "{"};
                        ++it;
                        operandStack.push(parseExpression(context, it, true));
                        if (!it->isSymbol() || it->getSymbol().name != "}")
                            throw Error{ErrorType::MISSING, "}"};
                        operatorStack.push(oi);
                        expectingOperand = false;
                        continue;
                    }

                    if ((oi.precedence == OperatorPrecedence::PREFIX) != expectingOperand) {
                        throw Error{ErrorType::UNEXPECTED, sym};
                    }

                    while (!operatorStack.empty() && isEvaluatedBefore(operatorStack.top(), oi)) {
                        popOperator(operatorStack, operandStack, context);
                    }

                    // Parse operator normally
                    switch (oi.operation) {
                        case Operation::CALL:
                            ++it;
                            if (it->isSymbol() && it->getSymbol().name == "\\right)")
                                break;
                            // Function call, treat children of operator as arguments
                            while (true) {
                                operandStack.push(parseExpression(context, it, false));
                                ++oi.numOperands;
                                if (it->isSymbol() && it->getSymbol().name == "\\right)")
                                    break;
                                if (!it->isSymbol() || it->getSymbol().name != ",")
                                    throw Error{ErrorType::MISSING, "\\right)"};
                                ++it;
                            }
                            break;

                        case Operation::INDEX:
                            ++it;
                            operandStack.push(parseExpression(context, it, false));
                            if (!it->isSymbol() || it->getSymbol().name != "\\right]")
                                throw Error{ErrorType::MISSING, "\\right]"};
                            break;

                        default:
                            break;
                    }

                    operatorStack.push(oi);
                    expectingOperand = oi.precedence != OperatorPrecedence::POSTFIX;

                } else {
                    // Parse an operand

                    if (!expectingOperand) {
                        OperatorInfo oi(Operation::MUL);
                        
                        while (!operatorStack.empty() && isEvaluatedBefore(operatorStack.top(), oi)) {
                            popOperator(operatorStack, operandStack, context);
                        }

                        operatorStack.push(oi);
                    }

                    Expression::Value value;

                    if (it->isNumber())
                        value = it->getNumber();
                    else if (it->isSymbol())
                        value = it->getSymbol();

                    operandStack.emplace(context, value, std::vector<Expression>());
                    expectingOperand = false;
                }
            }

            if (expectingOperand) {
                if (allowEmpty && operandStack.empty() && operatorStack.empty()) {
                    return Expression();
                }
                throw Error{ErrorType::EXPECTED_OPERAND, ""};
            }

            while (!operatorStack.empty()) {
                popOperator(operatorStack, operandStack, context);
            }

            if (operandStack.size() != 1 || !operatorStack.empty()) {
                throw Error{ErrorType::UNKNOWN_ERROR, ""};
            }

            return std::move(operandStack.top());
        }
        
    }
    

    Expression generateParseTree(GraphContext& context, TokenIterator& it, DataType type, bool allowEmpty, bool allowComma) {
        Expression ret = parseExpression(context, it, allowEmpty, allowComma);
        if (ret.getType() != type && type != DataType::NOTHING)
            throw Error{ErrorType::BAD_TYPE, ""}; // TODO: type names
        return ret;
    }

}