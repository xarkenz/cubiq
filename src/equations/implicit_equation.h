#pragma once

#include "equation.h"

class ImplicitEquation : public Equation {

public:

    ImplicitEquation(DisplaySettings settings, float (*func)(float,float));
    float apply(float x, float y) const; // Function drawn where apply(x,y)=0

    unsigned long getNumVertices(BoundingBox boundingBox, double precision) const override;
    void writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const override;

private:
    float (*function)(float,float);

};
