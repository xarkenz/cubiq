#include <cstring>
#include "graph.h"


const int Graph::NUM_THREADS = 2; // Number of threads to use for parallel computing

Graph::Graph(BoundingBox bb) : boundingBox(bb), equationList() {
    numVertices = 0;
    vertices = nullptr;
}

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

// Returns a copy of the internal vertex list
GLfloat* Graph::getVertices(unsigned long& numVerts) {
    std::scoped_lock<std::mutex> lock(mutex);

    numVerts = numVertices;
    if (numVertices == 0) { return nullptr; }

    GLfloat* verts = new GLfloat[numVertices*7];
    std::memcpy(verts,vertices,numVertices*7*sizeof(GLfloat));
    return verts;
}

// Returns the internal vertex list. Use with a mutex lock
GLfloat* Graph::getVertexList(unsigned long& numVerts) {
    numVerts = numVertices;
    return vertices;
}


void Graph::calculateVertices(double precision) {
    std::scoped_lock<std::mutex> lock(mutex);

    delete[] vertices;
    numVertices = 0;

    unsigned long* indexes = new unsigned long[equationList.size()+1];

    indexes[0] = 0;
    for (int i = 0; i < equationList.size(); i++) {
        indexes[i+1] = indexes[i] + equationList.at(i)->getNumVertices(boundingBox,precision);
    }

    numVertices = indexes[equationList.size()];
    vertices = new GLfloat[numVertices*7];

    #pragma omp parallel for num_threads(Graph::NUM_THREADS) shared(indexes,precision) default(none)
    for (int i = 0; i < equationList.size(); i++) {
        equationList.at(i)->writeVertices(vertices + 7*indexes[i], boundingBox, precision);
    }

    delete[] indexes;
}

void Graph::addEquation(Equation* e) {
    std::scoped_lock<std::mutex> lock(mutex);
    equationList.push_back(e);
}


std::mutex& Graph::getMutex() { return mutex; }
