#include <sstream>
#include <cmath>
#include <iostream>
#include "equation.h"
#include "function.h"
#include "implicit_equation.h"

namespace Cubiq {

    const int Equation::NUM_THREADS = 4;

    Equation::DisplaySettings Equation::getDisplaySettings() {
        return displaySettings;
    }

    GLfloat* Equation::getVertices(unsigned long& numVerts, BoundingBox boundingBox, double precision) const {
        numVerts = getNumVertices(boundingBox, precision);
        auto* vertices = new GLfloat[numVerts];
        writeVertices(vertices, boundingBox, precision);
        return vertices;
    }

    void Equation::writeVertex(GLfloat* vertices, int vertIndex, float x, float y) const {
        vertices[7 * vertIndex] = x;
        vertices[7 * vertIndex + 1] = y;
        vertices[7 * vertIndex + 2] = 0;
        vertices[7 * vertIndex + 3] = displaySettings.r;
        vertices[7 * vertIndex + 4] = displaySettings.g;
        vertices[7 * vertIndex + 5] = displaySettings.b;
        vertices[7 * vertIndex + 6] = displaySettings.a;
    }

    Equation* Equation::setFromString(std::string str) {
        std::istringstream reader(str);
        reader.seekg(0);
        Parser::CharStream stream = [&reader]() -> int {
            return reader.get();
        };
        Parser::Expression expression;

        try {
            Parser::TokenIterator it(stream);
            Parser::GraphContext context;
            expression = generateParseTree(context, it, Parser::DataType::NOTHING, true);
        } catch (Parser::Error err) {
            reader.seekg(0);
            // TODO: Eventually handle the error here
            return new EmptyEquation(displaySettings);
        }
        return setFromExpression(expression);
    }

    Equation* Equation::setFromExpression(Parser::Expression expr) {
        if (expr.getOperation() != Parser::Operation::EQ) {
            // TODO: Handle it not being an equality
            return new EmptyEquation(displaySettings);
        }

        int a = 0;
        // TODO: add support for Function
        std::function<float(float,float)> func = [expr](float x, float y) -> float {return eval(expr.getChildren().at(0),x,y) - eval(expr.getChildren().at(1),x,y);};
        return new ImplicitEquation(displaySettings, func);
    }

    float Equation::eval(Parser::Expression expr, float x, float y) {
        if (expr.isEmpty()) {
            if (expr.getChildren().size() == 0) {return 0;}
            else {return eval(expr.getChildren().at(0),x,y);} // TODO: idk if I need this
        }
        else if (expr.isNumber()) {return expr.getNumber();}
        else if (expr.isSymbol()) {
            std::string name = expr.getSymbol().name;
            if (name == "x") { return x; }
            else if (name == "y") { return y; }
            else {return 0;}
        }
        else if (expr.isOperation()) {
            switch(expr.getOperation()) {
                case Parser::Operation::ADD:
                    return eval(expr.getChildren().at(0),x,y) + eval(expr.getChildren().at(1),x,y);
                case Parser::Operation::SUB:
                    return eval(expr.getChildren().at(0),x,y) - eval(expr.getChildren().at(1),x,y);
                case Parser::Operation::MUL:
                    return eval(expr.getChildren().at(0),x,y) * eval(expr.getChildren().at(1),x,y);
                case Parser::Operation::DIV:
                    return eval(expr.getChildren().at(0),x,y) / eval(expr.getChildren().at(1),x,y);
                case Parser::Operation::POS:
                    return eval(expr.getChildren().at(0),x,y);
                case Parser::Operation::NEG:
                    return -eval(expr.getChildren().at(0),x,y);
                case Parser::Operation::EXP:
                    return powf(eval(expr.getChildren().at(0),x,y), eval(expr.getChildren().at(1),x,y));
                case Parser::Operation::SQRT:
                    return powf(eval(expr.getChildren().at(1),x,y), 1.0f/eval(expr.getChildren().at(0),x,y));
                case Parser::Operation::FACT:
                    return std::tgamma(1+eval(expr.getChildren().at(0),x,y));
            }
            return 0; // TODO: account for more things
        }
    }

    Equation* Equation::createFromString(DisplaySettings settings, std::string str) {
        EmptyEquation ee(settings);
        return ee.setFromString(str);

    }

    Equation* Equation::createFromExpression(DisplaySettings settings, Parser::Expression expr) {
        EmptyEquation ee(settings);
        return ee.setFromExpression(expr);

    }

    EmptyEquation::EmptyEquation(DisplaySettings settings) : Equation(settings) {}
    unsigned long EmptyEquation::getNumVertices(BoundingBox boundingBox, double precision) const {return 0;}
    void EmptyEquation::writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const {}

}