
#include <cmath>
#include "function.h"
#include <iostream>

Function::Function(DisplaySettings settings, Function::IndependentVariable inVar, float (*func)(float)) : Equation(settings) {
    inputVar = inVar;
    function = func;
}

float Function::apply(float input) {
    return (*function)(input);
}


GLfloat* Function::getVertices(int& numVerts, BoundingBox boundingBox, float precision) {

    float inMin = 0, inMax = 0, outMin = 0, outMax = 0;
    switch (inputVar) {
        case Function::IndependentVariable::X:
            inMin = std::floor(boundingBox.minX/precision)*precision;
            inMax = std::ceil(boundingBox.maxX/precision)*precision;
            outMin = std::floor(boundingBox.minY/precision)*precision;
            outMax = std::ceil(boundingBox.maxY/precision)*precision;
            break;
        case Function::IndependentVariable::Y:
            inMin = std::floor(boundingBox.minY/precision)*precision;
            inMax = std::ceil(boundingBox.maxY/precision)*precision;
            outMin = std::floor(boundingBox.minX/precision)*precision;
            outMax = std::ceil(boundingBox.maxX/precision)*precision;
            break;
    }

    int numSegments = (int) ((inMax - inMin)/precision);
    GLfloat* vertices = new GLfloat[7 * 2*numSegments];

    float in, out, x, y, prevX, prevY;
    int vertIndex = 0;
    bool inBounds,prevInBounds;
    for (int i = 0; i < numSegments+1; i++) {
        in = inMin + i*precision;
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

        inBounds = out <= outMax && out >= outMin;

        if (i > 0 && (prevInBounds || inBounds)) {
            writeVertex(vertices, vertIndex++, prevX, prevY);
            writeVertex(vertices, vertIndex++, x, y);
        }

        prevX = x;
        prevY = y;
        prevInBounds = inBounds;

    }

    numVerts = vertIndex;
    if (vertIndex%2 == 1) {
        std::cout << "oh no: " << displaySettings.r << ", " << displaySettings.g << ", " <<displaySettings.b << std::endl;
    }

    return vertices;
}
