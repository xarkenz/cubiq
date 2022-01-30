#pragma once

#include "equation.h"

class ImplicitEquation : public Equation {

public:

    ImplicitEquation(DisplaySettings settings, float (*func)(float,float));
    double apply(float x, float y); // Function drawn where apply(x,y)=0

    GLfloat* getVertices(int*& segIndices, int& numSegs, BoundingBox boundingBox, float precision) override;

private:
    float (*function)(float,float);

    void writeVertex(GLfloat* vertices, int vertIndex,float x, float y);

};
