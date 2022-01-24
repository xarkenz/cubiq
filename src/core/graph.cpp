#include "graph.h"

Graph::Graph(BoundingBox bb) {
    boundingBox = bb;
    vertices = std::vector<GLfloat>();
    equationList = std::vector<Equation*>();
}

Graph::Graph() : Graph(BoundingBox{-10,10,-10,10}) {}


BoundingBox Graph::getBoundingBox() {
    return boundingBox;
}

GLfloat* Graph::getVertices(int &length) {
    length = vertices.size() / 7;
    return &vertices[0];
}


void Graph::calculateVertices(float precision) {
    vertices.clear();

    int num;
    GLfloat* verts;
    for (Equation* e : equationList) {
        verts = e->getVertices(num, boundingBox, precision);
        vertices.insert(vertices.end(), verts, verts + num*7);
        delete verts;
    }
}

void Graph::addEquation(Equation* e) {
    equationList.insert(equationList.end(),e);
}

