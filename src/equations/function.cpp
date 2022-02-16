#include "function.h"

#include <cmath>


namespace Cubiq {

    Function::Function(DisplaySettings settings, Function::IndependentVariable inVar, std::function<float(float)> func) : Equation(
            settings) {
        inputVar = inVar;
        function = std::move(func);
    }


    float Function::apply(float input) const {
        return function(input);
    }


    unsigned long Function::getNumVertices(BoundingBox boundingBox, double precision) const {
        double inMin = 0, inMax = 0;
        switch (inputVar) {
            case Function::IndependentVariable::X:
                inMin = std::floor(boundingBox.minX / precision) * precision;
                inMax = std::ceil(boundingBox.maxX / precision) * precision;
                break;
            case Function::IndependentVariable::Y:
                inMin = std::floor(boundingBox.minY / precision) * precision;
                inMax = std::ceil(boundingBox.maxY / precision) * precision;
                break;
        }

        if (inMax <= inMin) { return 0; }
        return 2 * (unsigned long) ((inMax - inMin) / precision);
    }

    void Function::writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const {

        unsigned long numVerts = getNumVertices(boundingBox, precision);

        if (numVerts == 0) { return; }

        double inMin = 0, outMin = 0, outMax = 0;
        switch (inputVar) {
            case Function::IndependentVariable::X:
                inMin = std::floor(boundingBox.minX / precision) * precision;
                outMin = std::floor(boundingBox.minY / precision) * precision;
                outMax = std::ceil(boundingBox.maxY / precision) * precision;
                break;
            case Function::IndependentVariable::Y:
                inMin = std::floor(boundingBox.minY / precision) * precision;
                outMin = std::floor(boundingBox.minX / precision) * precision;
                outMax = std::ceil(boundingBox.maxX / precision) * precision;
                break;
        }

        unsigned long numSegments = numVerts / 2;

        float in, out, x, y, prevX, prevY;
        int vertIndex = 0;
        bool inBounds, prevInBounds;
        for (long i = 0; i < numSegments + 1; i++) {
            in = (float) (inMin + (double) i * precision);
            out = apply(in);

            switch (inputVar) {
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

        while (vertIndex < numVerts) { writeVertex(vertices, vertIndex++, 0, 0); }

    }

}