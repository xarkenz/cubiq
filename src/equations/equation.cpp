#include "equation.h"

const int Equation::NUM_THREADS = 4;


GLfloat* Equation::getVertices(unsigned long& numVerts, BoundingBox boundingBox, double precision) const {
    numVerts = getNumVertices(boundingBox,precision);
    GLfloat* vertices = new GLfloat[numVerts];
    writeVertices(vertices, boundingBox, precision);
    return vertices;
}

void Equation::writeVertex(GLfloat* vertices, int vertIndex, float x, float y) const {
    vertices[7*vertIndex] = x;
    vertices[7*vertIndex+1] = y;
    vertices[7*vertIndex+2] = 0;
    vertices[7*vertIndex+3] = displaySettings.r;
    vertices[7*vertIndex+4] = displaySettings.g;
    vertices[7*vertIndex+5] = displaySettings.b;
    vertices[7*vertIndex+6] = displaySettings.a;
}
