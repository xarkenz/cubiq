#include "graph.h"

Graph::Graph(BoundingBox bb) : boundingBox(bb), vertices(), equationList() {}

Graph::Graph() : Graph(BoundingBox{-10,10,-10,10}) {}

Graph::~Graph() {
    std::scoped_lock<std::mutex> lock(mutex);
    for (Equation* e : equationList) {
        delete e;
    }
}


BoundingBox Graph::getBoundingBox() {
    return boundingBox;
}

void Graph::setBoundingBox(BoundingBox bb) {
    std::scoped_lock<std::mutex> lock(mutex);
    boundingBox = bb;
}

// segIndices is a list of vertex indices representing where each segment ends
// The last element will be the total number of vertices
GLfloat* Graph::getVertices(int& numVerts) {
    std::scoped_lock<std::mutex> lock(mutex);

    numVerts = vertices.size()/7;
    GLfloat* verts;

    if (numVerts > 0) {
        verts = new GLfloat[vertices.size()];
        std::memcpy(verts, &vertices[0], sizeof(GLfloat) * vertices.size());
    } else {
        verts = nullptr;
    }

    return verts;
}


void Graph::calculateVertices(float precision) {
    std::scoped_lock<std::mutex> lock(mutex);

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
    std::scoped_lock<std::mutex> lock(mutex);
    equationList.push_back(e);
}

