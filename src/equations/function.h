#pragma once

#include "equation.h"


namespace Cubiq {

    class Function : public Equation {

    public:
        enum class IndependentVariable {
            X, Y
        };

        Function(DisplaySettings settings, IndependentVariable inVar, float (* func)(float));

        unsigned long getNumVertices(BoundingBox boundingBox, double precision) const override;
        void writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const override;

        float apply(float input) const;

    private:
        IndependentVariable inputVar;
        float (* function)(float);

    };

}