#pragma once

#include <vector>
#include <mutex>
#include <QOpenGLBuffer>
#include "equations/equation.h"
#include "core/bounding_box.h"


class Graph {

public:

    static const int NUM_THREADS;

    GLfloat* getVertices(unsigned long& numVerts);
    GLfloat* getVertexList(unsigned long& numVerts);
    void calculateVertices(double precision);
    BoundingBox getBoundingBox();
    void setBoundingBox(BoundingBox bb);

    std::mutex& getMutex();

    explicit Graph(BoundingBox bb);
    Graph();
    ~Graph();

    void addEquation(Equation* e);

private:
    std::mutex mutex;

    std::vector<Equation*> equationList;

    GLfloat* vertices;
    unsigned long numVertices;

    BoundingBox boundingBox;

};

