#pragma once

#include <QOpenGLBuffer>
#include "core/bounding_box.h"

#define NUM_THREADS 4 // Number of threads to use for parallel computing

class Equation {

public:

    struct DisplaySettings {
        float r, g, b, a;
    };


    explicit Equation(DisplaySettings settings) {displaySettings = settings;}
    virtual ~Equation(){};

    virtual GLfloat* getVertices(unsigned long& numVerts, BoundingBox boundingBox, float precision) = 0;

protected:
    DisplaySettings displaySettings{};

    void writeVertex(GLfloat* vertices, int vertIndex, float x, float y) {
        vertices[7*vertIndex] = x;
        vertices[7*vertIndex+1] = y;
        vertices[7*vertIndex+2] = 0;
        vertices[7*vertIndex+3] = displaySettings.r;
        vertices[7*vertIndex+4] = displaySettings.g;
        vertices[7*vertIndex+5] = displaySettings.b;
        vertices[7*vertIndex+6] = displaySettings.a;
    }

};
