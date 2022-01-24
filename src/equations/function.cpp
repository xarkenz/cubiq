
#include <cmath>
#include "function.h"

Function::Function(Function::IndependentVariable inVar, float (*func)(float)) {
    inputVar = inVar;
    function = func;
}

double Function::apply(float input) {
    return (*function)(input);
}


GLfloat* Function::getVertices(int &length, BoundingBox boundingBox, float precision) {

    float inMin = 0, inMax = 0;
    switch(inputVar) {
        case Function::IndependentVariable::X:
            inMin = std::floor(boundingBox.minX);
            inMax = std::ceil(boundingBox.maxX);
            break;
        case Function::IndependentVariable::Y:
            inMin = std::floor(boundingBox.minY);
            inMax = std::ceil(boundingBox.maxY);
            break;
    }

    length = std::floor((inMax - inMin + 1) / precision);
    GLfloat* vertices = new GLfloat[7 * length];

    float in, out, x, y;
    for (int i = 0; i < length; i++) {
        in = inMin + i * precision;
        out = apply(in);

        switch(inputVar) {
            case Function::IndependentVariable::X:
                x = in;
                y = out;
                break;
            case Function::IndependentVariable::Y:
                y = in;
                x = out;
                break;
        }

        vertices[7*i] = x;
        vertices[7*i+1] = y;
        vertices[7*i+2] = 0;
        vertices[7*i+3] = 0.8f;
        vertices[7*i+4] = 0.2f;
        vertices[7*i+5] = 0.1f;
        vertices[7*i+6] = 1.0f;
    }

    return vertices;
}
