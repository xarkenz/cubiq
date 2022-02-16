#pragma once

#include <vector>
#include <mutex>
#include <QOpenGLBuffer>
#include <QString>

#include "equations/equation.h"
#include "core/bounding_box.h"


namespace Cubiq {

    class Graph {

    public:
        static const int NUM_THREADS;

        GLfloat* getVertices(unsigned long& numVerts);
        GLfloat* getVertexList(unsigned long& numVerts);
        void calculateVertices(double precision);

        BoundingBox getBoundingBox();
        void setBoundingBox(BoundingBox bb);

        std::mutex& getMutex();

        explicit Graph(BoundingBox bb);
        Graph();
        ~Graph();

        void addEquation(Equation*& e); //TODO: make it not use a reference

        QString getName() const;
        QString getDescription() const;
        QString getAuthor() const;
        void setName(QString s);
        void setDescription(QString s);
        void setAuthor(QString s);

    private:
        QString name, description, author;
        bool grid;

        std::mutex mutex;

        std::vector<Equation*> equationList;

        GLfloat* vertices;
        unsigned long numVertices;

        BoundingBox boundingBox;

    };

}