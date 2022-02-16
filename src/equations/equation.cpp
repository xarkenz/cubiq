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
        //std::cout << str << std::endl;
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
        if (!expr.isOperation() || expr.getOperation() != Parser::Operation::EQ) {
            // TODO: Handle it not being an equality
            return new EmptyEquation(displaySettings);
        }

        int a = 0;
        // TODO: add support for Function
        return new ImplicitEquation(displaySettings, expr);
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

    unsigned long EquationWrapper::getNumVertices(BoundingBox boundingBox, double precision) const {return inner->getNumVertices(boundingBox, precision);}
    void EquationWrapper::writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const {inner->writeVertices(vertices,boundingBox, precision);}


    EquationWrapper::EquationWrapper(Equation* eq) {
        inner = eq;
    }

    Equation* EquationWrapper::getEquation() const {
        return inner;
    }
    void EquationWrapper::setEquation(Equation* eq) {
        Equation* temp = inner;
        inner = eq;
        delete temp;
    }

}