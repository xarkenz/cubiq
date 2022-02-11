#pragma once

#include <QOpenGLBuffer>

#include "core/bounding_box.h"


namespace Cubiq {

    class Equation {

    public:
        struct DisplaySettings {
            float r, g, b, a;
        };

        static const int NUM_THREADS;

        explicit Equation(DisplaySettings settings) { displaySettings = settings; }
        virtual ~Equation() {};

        GLfloat* getVertices(unsigned long& numVerts, BoundingBox boundingBox, double precision) const;
        virtual unsigned long getNumVertices(BoundingBox boundingBox, double precision) const = 0;
        virtual void writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const = 0;

    protected:
        DisplaySettings displaySettings{};

        void writeVertex(GLfloat* vertices, int vertIndex, float x, float y) const;

    };

}