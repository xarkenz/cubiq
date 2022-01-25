#include "graph.h"

Graph::Graph(BoundingBox bb) : boundingBox(bb), vertices(), segmentIndices(), equationList() {}

Graph::Graph() : Graph(BoundingBox{-10,10,-10,10}) {}


BoundingBox Graph::getBoundingBox() {
    return boundingBox;
}

// segIndices is a list of vertex indices representing where each segment ends
// The last element will be the total number of vertices
GLfloat* Graph::getVertices(int*& segIndices, int& numSegs) {
    segIndices = &segmentIndices[0];
    numSegs = segmentIndices.size();
    return &vertices[0];
}


void Graph::calculateVertices(float precision) {
    vertices.clear();
    segmentIndices.clear();

    int numSegs, vertCount;
    GLfloat* verts;
    int* segIndices;
    for (Equation* e : equationList) {
        verts = e->getVertices(segIndices, numSegs, boundingBox, precision);

        vertCount = segIndices[numSegs-1];

        for (int i = 0; i < numSegs; i++) {segIndices[i] += vertices.size()/7;}

        vertices.insert(vertices.end(), verts, verts + vertCount*7);
        segmentIndices.insert(segmentIndices.end(), segIndices, segIndices+numSegs);

        delete verts;
        delete segIndices;
    }
}

void Graph::addEquation(Equation* e) {
    equationList.push_back(e);
}

