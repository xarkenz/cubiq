#include <cstring>
#include <omp.h>
#include "graph.h"

#define NUM_THREADS 4 // Number of threads to use for parallel computing

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
GLfloat* Graph::getVertices(unsigned long& numVerts) {
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

    GLfloat** verts = new GLfloat*[equationList.size()];
    unsigned long* numVerts = new unsigned long[equationList.size()];

    #pragma omp parallel for num_threads(NUM_THREADS) shared(verts,numVerts,precision) default(none)
    for (int i = 0; i < equationList.size(); i++) {
        verts[i] = equationList.at(i)->getVertices(numVerts[i], boundingBox, precision);
    }

    for (int i = 0; i < equationList.size(); i++) {
        if (numVerts[i] > 0) { vertices.insert(vertices.end(), verts[i], verts[i] + numVerts[i] * 7); }
        delete[] verts[i];
    }

    delete[] verts;
    delete[] numVerts;
}

void Graph::addEquation(Equation* e) {
    std::scoped_lock<std::mutex> lock(mutex);
    equationList.push_back(e);
}

