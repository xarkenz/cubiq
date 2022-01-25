#pragma once

#include <QOpenGLBuffer>
#include "core/bounding_box.h"

class Equation {

    // TODO: add color

public:

    struct DisplaySettings {
        float r, g, b, a;
    };


    explicit Equation(DisplaySettings settings) {displaySettings = settings;}
    virtual ~Equation(){};

    virtual GLfloat* getVertices(int*& segIndices, int& numSegs, BoundingBox boundingBox, float precision) = 0;

protected:
    DisplaySettings displaySettings{};

};
