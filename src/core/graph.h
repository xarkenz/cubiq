#pragma once

#include <vector>
#include <QOpenGLBuffer>
#include "equations/equation.h"
#include "core/bounding_box.h"


class Graph {

public:


    GLfloat* getVertices(int &length);
    void calculateVertices(float precision);
    BoundingBox getBoundingBox();

    Graph(BoundingBox bb);
    Graph();

    void addEquation(Equation* e);

private:
    std::vector<Equation*> equationList;

    std::vector<GLfloat> vertices;

    BoundingBox boundingBox;

};

