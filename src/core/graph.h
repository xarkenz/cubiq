#pragma once

#include <vector>
#include <mutex>
#include <QOpenGLBuffer>
#include "equations/equation.h"
#include "core/bounding_box.h"


class Graph {

public:


    GLfloat* getVertices(int& numVerts);
    void calculateVertices(float precision);
    BoundingBox getBoundingBox();
    void setBoundingBox(BoundingBox bb);

    explicit Graph(BoundingBox bb);
    Graph();
    ~Graph();

    void addEquation(Equation* e);

private:
    std::mutex mutex;

    std::vector<Equation*> equationList;

    std::vector<GLfloat> vertices;

    BoundingBox boundingBox;

};

