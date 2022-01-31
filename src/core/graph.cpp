#include "graph.h"

Graph::Graph(BoundingBox bb) : boundingBox(bb), vertices(), equationList() {}

Graph::Graph() : Graph(BoundingBox{-10,10,-10,10}) {}


BoundingBox Graph::getBoundingBox() {
    return boundingBox;
}

void Graph::setBoundingBox(BoundingBox bb) {
    boundingBox = bb;
}

// segIndices is a list of vertex indices representing where each segment ends
// The last element will be the total number of vertices
GLfloat* Graph::getVertices(int& numVerts) {
    numVerts = vertices.size()/7;
    return &vertices[0];
}


void Graph::calculateVertices(float precision) {
    vertices.clear();

    int numVerts;
    GLfloat* verts;
    for (Equation* e : equationList) {
        verts = e->getVertices(numVerts, boundingBox, precision);
        if (numVerts > 0) { vertices.insert(vertices.end(), verts, verts + numVerts * 7); }
        delete verts;
    }
}

void Graph::addEquation(Equation* e) {
    equationList.push_back(e);
}

