#pragma once

#include "equation.h"


namespace Cubiq {

    class ImplicitEquation : public Equation {

    public:
        //ImplicitEquation(DisplaySettings settings, std::function<float(float,float)>);
        ImplicitEquation(DisplaySettings settings, Parser::Expression expr);

        float apply(float x, float y) const; // Function drawn where apply(x,y)=0
        // TODO: move to Parser
        static float eval(const Parser::Expression* expr, float x, float y);

        unsigned long getNumVertices(BoundingBox boundingBox, double precision) const override;
        void writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const override;

    private:
        //std::function<float(float,float)> function; //TODO: revert to this
        Parser::Expression expression;
    };

}