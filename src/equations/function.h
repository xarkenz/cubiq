#pragma once

#include "equations/equation.h"

class Function : public Equation {

public:

    enum class IndependentVariable {X, Y};

    Function(IndependentVariable inVar, float (*func)(float));

    GLfloat* getVertices(int &length, BoundingBox boundingBox, float precision) override;

    double apply(float input);

private:
    IndependentVariable inputVar;
    float (*function)(float);

};
