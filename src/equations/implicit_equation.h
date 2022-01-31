#pragma once

#include "equation.h"

class ImplicitEquation : public Equation {

public:

    ImplicitEquation(DisplaySettings settings, float (*func)(float,float));
    float apply(float x, float y); // Function drawn where apply(x,y)=0

    GLfloat* getVertices(int& numVerts, BoundingBox boundingBox, float precision) override;

private:
    float (*function)(float,float);

};
