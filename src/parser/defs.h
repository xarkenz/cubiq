#pragma once

#include <string>


namespace Cubiq::Parser {

    enum struct DataType {
        UNRESOLVED,
        NOTHING,
        NUMBER,
        BOOLEAN,
        POINT,
        ARRAY,
        FUNCTION,
        ACTION,
    };


    enum struct Operation {
        // Arithmetic
        POS, NEG,
        ADD, SUB,
        MUL, DIV, MOD,
        EXP, SQRT,
        FACT,

        // Logical
        L_NOT,
        L_AND, L_OR, L_XOR,

        // Bitwise
        B_NOT,
        B_AND, B_OR, B_XOR,
        B_LSHIFT, B_RSHIFT,

        // Comparison
        EQ, NEQ,
        LT, GT,
        LTEQ, GTEQ,

        // Structural
        COMMA,
        CALL,
        INDEX,
        POINT,
        ARRAY,
        BRANCH,
    };
    

    enum struct ErrorType {
        UNKNOWN_ERROR,      // Source of error is unknown
        UNEXPECTED,         // Unexpected character or token
        UNEXPECTED_OPERAND, // Expected an operator, but got an operand
        EXPECTED_OPERAND,   // Expected an operator, but none was found
        MISSING,            // Needed token could not be found
        BAD_TYPE,           // Encountered wrong type
    };


    struct Error {
        ErrorType type;
        std::string content;
    };

}