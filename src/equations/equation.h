#pragma once

#include <QOpenGLBuffer>

#include "core/bounding_box.h"
#include "parser/interpreter.h"


namespace Cubiq {

    class Equation {

    public:
        struct DisplaySettings {
            float r, g, b, a;
        };

        static const int NUM_THREADS;

        explicit Equation(DisplaySettings settings) { displaySettings = settings; }
        virtual ~Equation() {};

        DisplaySettings getDisplaySettings();

        GLfloat* getVertices(unsigned long& numVerts, BoundingBox boundingBox, double precision) const;
        virtual unsigned long getNumVertices(BoundingBox boundingBox, double precision) const = 0;
        virtual void writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const = 0;


        Equation* setFromString(std::string str);
        Equation* setFromExpression(Parser::Expression expr);

        static Equation* createFromString(DisplaySettings settings, std::string str);
        static Equation* createFromExpression(DisplaySettings settings, Parser::Expression expr);

        // TODO: move to Parser
        static float eval(Parser::Expression expr, float x, float y);

    protected:
        DisplaySettings displaySettings{};

        void writeVertex(GLfloat* vertices, int vertIndex, float x, float y) const;

    };

    class EmptyEquation : public Equation {
        public:
            EmptyEquation(DisplaySettings settings);

            unsigned long getNumVertices(BoundingBox boundingBox, double precision) const override;
            void writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const override;
    };

}