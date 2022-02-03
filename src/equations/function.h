#pragma once

#include "equations/equation.h"

class /*[[deprecated("Use ImplicitEquation instead.")]]*/ Function : public Equation {

public:

    enum class IndependentVariable {X, Y};

    Function(DisplaySettings settings, IndependentVariable inVar, float (*func)(float));

    GLfloat* getVertices(int& numVerts, BoundingBox boundingBox, float precision) override;

    float apply(float input);

private:
    IndependentVariable inputVar;
    float (*function)(float);

};
