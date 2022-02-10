#pragma once

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include <thread>

#include "core/graph.h"


class GraphView : public QOpenGLWidget, protected QOpenGLExtraFunctions {

private:
    class CalculationThread : public std::thread {

    public:
        static const int MILLIS_PER_UPDATE;

        CalculationThread(GraphView* p, Graph* g);

        void markToUpdate();
        void markToExit();

        void setGraph(Graph* g);

    private:

        void run();

        bool toUpdate;
        bool toExit;

        GraphView* parent;
        Graph* graph;

    };


public:
    static const int BATCH_SIZE;

    GraphView(QWidget* parent, Graph* g);
    ~GraphView();

    void setGraph(Graph* g);
    Graph* getGraph();

    void centerOrigin();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void drawGrid();
    void drawElements();

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void zoom(float steps, QPointF pos);
    void adjustCamera();

    GLuint createShader(const char* vertexSource, const char* fragmentSource, int attribCount, const char* attribs[]);

private:

    float clearR, clearG, clearB;
    int screenW, screenH;

    Graph* graph;
    CalculationThread calculationThread;

    QMatrix4x4 projection;

    GLuint shaderProgram{};
    GLuint vertexArray{};
    GLuint vertexBuffer{};

    GLint bufferIndex;

    QPoint dragStartPos;
    BoundingBox dragStartBounds;
    bool dragging;

};
