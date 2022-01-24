#pragma once

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>


class GraphView : public QOpenGLWidget, protected QOpenGLExtraFunctions {

public:
    GraphView(QWidget* parent);

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void drawGrid();

    void drawElements();

    void adjustCamera();

    GLuint createShader(const char* vertexSource, const char* fragmentSource, int attribCount, const char* attribs[]);

private:
    float clearR, clearG, clearB;
    int screenW, screenH;
    float graphL, graphR, graphB, graphT;
    QMatrix4x4 projection;

    GLuint shaderProgram{};
    GLuint vertexArray{};
    GLuint vertexBuffer{};

    GLint bufferIndex;

};
