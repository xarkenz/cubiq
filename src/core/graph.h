#pragma once

#include <vector>
#include <QOpenGLBuffer>
#include "equations/equation.h"
#include "core/bounding_box.h"


class Graph {

public:


    GLfloat* getVertices(int*& segIndices, int& numSegs);
    void calculateVertices(float precision);
    BoundingBox getBoundingBox();
    void setBoundingBox(BoundingBox bb);

    explicit Graph(BoundingBox bb);
    Graph();

    void addEquation(Equation* e);

private:
    std::vector<Equation*> equationList;

    std::vector<GLfloat> vertices;
    std::vector<int> segmentIndices;

    BoundingBox boundingBox;

};

