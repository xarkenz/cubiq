#pragma once

#include "equations/equation.h"

class Function : public Equation {

public:

    enum class IndependentVariable {X, Y};

    Function(DisplaySettings settings, IndependentVariable inVar, float (*func)(float));

    GLfloat* getVertices(int*& segIndices, int& numSegs, BoundingBox boundingBox, float precision) override;

    double apply(float input);

private:
    IndependentVariable inputVar;
    float (*function)(float);

};
