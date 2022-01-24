#pragma once

#include <QOpenGLBuffer>
#include "core/bounding_box.h"

class Equation {

    // TODO: add color

public:
    virtual ~Equation(){};

    virtual GLfloat* getVertices(int &length, BoundingBox boundingBox, float precision) = 0;

};
