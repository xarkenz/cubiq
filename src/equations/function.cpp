
#include <cmath>
#include "function.h"

Function::Function(DisplaySettings settings, Function::IndependentVariable inVar, float (*func)(float)) : Equation(settings) {
    inputVar = inVar;
    function = func;
}

double Function::apply(float input) {
    return (*function)(input);
}


GLfloat* Function::getVertices(int*& segIndices, int& numSegs, BoundingBox boundingBox, float precision) {

    float inMin = 0, inMax = 0, outMin = 0, outMax = 0;
    switch(inputVar) {
        case Function::IndependentVariable::X:
            inMin = std::floor(boundingBox.minX);
            inMax = std::ceil(boundingBox.maxX);
            outMin = std::floor(boundingBox.minY);
            outMax = std::ceil(boundingBox.maxY);
            break;
        case Function::IndependentVariable::Y:
            inMin = std::floor(boundingBox.minY);
            inMax = std::ceil(boundingBox.maxY);
            outMin = std::floor(boundingBox.minX);
            outMax = std::ceil(boundingBox.maxX);
            break;
    }

    int numVertices = std::floor((inMax - inMin + 1) / precision);
    GLfloat* vertices = new GLfloat[7 * numVertices];

    segIndices = new int[numVertices/2]; // Allocating more space than needed
    numSegs = 0;

    float in, out, nextOut, x, y;
    int vertIndex = 0;
    bool outOfBounds = false;
    for (int i = 0; i < numVertices; i++) {
        in = inMin + i * precision;
        out = apply(in);

        if (out > outMax || out < outMin) {
            nextOut = apply(in+precision);
            if (nextOut <= outMax && nextOut >= outMin) {
                outOfBounds = false;
            } else {
                if (outOfBounds) {
                    continue;
                }
                outOfBounds = true;
                segIndices[numSegs++] = vertIndex + 1;
            }
        } else {
            outOfBounds = false;
        }

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

        vertices[7*vertIndex] = x;
        vertices[7*vertIndex+1] = y;
        vertices[7*vertIndex+2] = 0;
        vertices[7*vertIndex+3] = displaySettings.r;
        vertices[7*vertIndex+4] = displaySettings.g;
        vertices[7*vertIndex+5] = displaySettings.b;
        vertices[7*(vertIndex++)+6] = displaySettings.a;
    }

    segIndices[numSegs++] = vertIndex;

    return vertices;
}
