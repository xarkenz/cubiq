#pragma once

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include "core/graph.h"


class GraphView : public QOpenGLWidget, protected QOpenGLExtraFunctions {

public:
    GraphView(QWidget* parent, Graph* g);

    void setGraph(Graph* g);
    Graph* getGraph();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void drawGrid();
    void drawElements();

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void zoom(float steps, QPointF pos);
    void adjustCamera();

    GLuint createShader(const char* vertexSource, const char* fragmentSource, int attribCount, const char* attribs[]);

private:
    float clearR, clearG, clearB;
    int screenW, screenH;

    Graph* graph;

    QMatrix4x4 projection;

    GLuint shaderProgram{};
    GLuint vertexArray{};
    GLuint vertexBuffer{};

    GLint bufferIndex;

    QPoint dragStartPos;
    BoundingBox dragStartBounds;

};
