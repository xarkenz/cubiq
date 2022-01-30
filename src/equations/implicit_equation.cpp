#include <cmath>
#include "implicit_equation.h"
#include <iostream>

ImplicitEquation::ImplicitEquation(DisplaySettings settings, float (*func)(float,float)) : Equation(settings) {
    function = func;
}

double ImplicitEquation::apply(float x, float y) {
    return (*function)(x,y);
}

void ImplicitEquation::writeVertex(GLfloat* vertices, int vertIndex, float x, float y) {
    vertices[7*vertIndex] = x;
    vertices[7*vertIndex+1] = y;
    vertices[7*vertIndex+2] = 0;
    vertices[7*vertIndex+3] = displaySettings.r;
    vertices[7*vertIndex+4] = displaySettings.g;
    vertices[7*vertIndex+5] = displaySettings.b;
    vertices[7*(vertIndex++)+6] = displaySettings.a;
}

GLfloat* ImplicitEquation::getVertices(int*& segIndices, int& numSegs, BoundingBox boundingBox, float precision) {
    int gridWidth = std::ceil(boundingBox.maxX/precision)-std::floor(boundingBox.minX/precision);
    int gridHeight = std::ceil(boundingBox.maxY/precision)-std::floor(boundingBox.minY/precision);
    int numVertices = 4 * gridWidth*gridHeight; // At most 4 vertices per cell
    GLfloat* vertices = new GLfloat[7 * numVertices];

    segIndices = new int[numVertices/2]; // 1 segment per 2 vertices
    numSegs = 0;

    // Calculate values
    float values[gridHeight+1][gridWidth+1];
    float x,y;
    for (int xInd = 0; xInd<gridWidth+1; xInd++) {
        x = (std::floor(boundingBox.minX/precision) + xInd)*precision;
        for (int yInd = 0; yInd<gridHeight+1; yInd++) {
            y = (std::floor(boundingBox.minY/precision) + yInd)*precision;
            values[yInd][xInd] = apply(x,y);
        }
    }

    // Create vertices
    int vertIndex = 0;
    float tl,tr,bl,br;
    float l,r,b,t,c;
    float v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;
    bool v1,v2,v3,v4;
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {

            tl = values[y+1][x];
            tr = values[y+1][x+1];
            bl = values[y][x];
            br = values[y][x+1];

            if ((tl > 0 and tr > 0 and bl > 0 and br > 0) or (tl < 0 and tr < 0 and bl < 0 and br < 0) or (tl == 0 and tr == 0 and bl == 0 and br == 0)) {
                continue;
            }

            v1 = v2 = v3 = v4 = false;

            l = (std::floor(boundingBox.minX/precision) + x)*precision;
            r = l + precision;
            b = (std::floor(boundingBox.minY/precision) + y)*precision;
            t = b + precision;


            if (bl == 0 and br == 0) {
                writeVertex(vertices, vertIndex++, l, b);
                writeVertex(vertices, vertIndex++, r, b);
                segIndices[numSegs++] = vertIndex;
            }
            if (tl == 0 and bl == 0) {
                writeVertex(vertices, vertIndex++, l, t);
                writeVertex(vertices, vertIndex++, l, b);
                segIndices[numSegs++] = vertIndex;
            }


            if ((tl > 0 and tr < 0) or (tl < 0 and tr > 0)) {
                v1 = true;
                v1y = t;
                v1x = (tl/(tl-tr))*precision + l;
            }
            if ((bl > 0 and br < 0) or (bl < 0 and br > 0)) {
                v2 = true;
                v2y = b;
                v2x = (bl/(bl-br))*precision + l;
            }

            if ((bl > 0 and tl < 0) or (bl < 0 and tl > 0)) {
                v3 = true;
                v3x = l;
                v3y = (bl/(bl-tl))*precision + b;
            }

            if ((br > 0 and tr < 0) or (br < 0 and tr > 0)) {
                v4 = true;
                v4x = r;
                v4y = (br/(br-tr))*precision + b;
            }

            if (v1 and v2 and not v3 and not v4) {
                writeVertex(vertices, vertIndex++, v1x, v1y);
                writeVertex(vertices, vertIndex++, v2x, v2y);
                segIndices[numSegs++] = vertIndex;
            } else if (v1 and v3 and not v2 and not v4) {
                writeVertex(vertices, vertIndex++, v1x, v1y);
                writeVertex(vertices, vertIndex++, v3x, v3y);
                segIndices[numSegs++] = vertIndex;
            } else if (v1 and v4 and not v2 and not v3) {
                writeVertex(vertices, vertIndex++, v1x, v1y);
                writeVertex(vertices, vertIndex++, v4x, v4y);
                segIndices[numSegs++] = vertIndex;
            } else if (v2 and v3 and not v1 and not v4) {
                writeVertex(vertices, vertIndex++, v2x, v2y);
                writeVertex(vertices, vertIndex++, v3x, v3y);
                segIndices[numSegs++] = vertIndex;
            } else if (v2 and v4 and not v1 and not v3) {
                writeVertex(vertices, vertIndex++, v2x, v2y);
                writeVertex(vertices, vertIndex++, v4x, v4y);
                segIndices[numSegs++] = vertIndex;
            } else if (v3 and v4 and not v1 and not v2) {
                writeVertex(vertices, vertIndex++, v3x, v3y);
                writeVertex(vertices, vertIndex++, v4x, v4y);
                segIndices[numSegs++] = vertIndex;
            } else if (v1 and v2 and v3 and v4) {
                c = apply((l+r)/2,(b+t)/2);
                if (c * tl >= 0) {
                    writeVertex(vertices, vertIndex++, v1x, v1y);
                    writeVertex(vertices, vertIndex++, v4x, v4y);
                    segIndices[numSegs++] = vertIndex;

                    writeVertex(vertices, vertIndex++, v2x, v2y);
                    writeVertex(vertices, vertIndex++, v3x, v3y);
                    segIndices[numSegs++] = vertIndex;
                } else {
                    writeVertex(vertices, vertIndex++, v1x, v1y);
                    writeVertex(vertices, vertIndex++, v3x, v3y);
                    segIndices[numSegs++] = vertIndex;

                    writeVertex(vertices, vertIndex++, v2x, v2y);
                    writeVertex(vertices, vertIndex++, v4x, v4y);
                    segIndices[numSegs++] = vertIndex;
                }
            }

        }
    }

    return vertices;
}
