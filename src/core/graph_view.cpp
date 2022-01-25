#include "graph_view.h"

#include <cmath>
#include <iostream>

#include <QWheelEvent>

#define BATCH_SIZE 4000

const GLsizei VERTEX_BYTES = 7 * sizeof(GLfloat);

const char* BASIC_VERTEX =
#include "shader/basic_vertex_glsl.h"

const char* BASIC_FRAGMENT =
#include "shader/basic_fragment_glsl.h"


GraphView::GraphView(QWidget* parent, Graph* g) : QOpenGLWidget(parent) {
        graph = g;

        clearR = .1f;
        clearG = .1f;
        clearB = .1f;

        screenW = 0;
        screenH = 0;

        bufferIndex = 0;
}

void GraphView::setGraph(Graph* g) {graph = g;}
Graph* GraphView::getGraph() {return graph;}


void GraphView::initializeGL() {
    initializeOpenGLFunctions();

    // Configure rendering
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Configure screen clear color
    glClearColor(clearR, clearG, clearB, 1);

    // Compile and link shader program
    const char* attribs[] = {"vPos", "vColor"};
    shaderProgram = createShader(BASIC_VERTEX, BASIC_FRAGMENT, 2, attribs);
    glUseProgram(shaderProgram);

    // Generate vertex array object
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Generate vertex buffer object
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // Configure vertex attributes (0 = vec3 vPos, 1 = vec4 vColor)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*) (0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));

    // Unbind VAO/VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void GraphView::resizeGL(int w, int h) {
    screenW = w;
    screenH = h;

    setMinimumWidth(h / 8);

    adjustCamera();
}


void GraphView::paintGL() {
    initializeOpenGLFunctions();

    // Prepare the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Upload projection matrix to shader
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, projection.data());

    // Prepare VAO/VBO
    glBindVertexArray(vertexArray);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, BATCH_SIZE * VERTEX_BYTES, NULL, GL_DYNAMIC_DRAW);

    // Load geometry onto the buffer and draw
    bufferIndex = 0;
    drawGrid();
    drawElements();

    // Unbind VAO/VBO
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Stop using shader program
    glUseProgram(0);
}


void GraphView::drawGrid() {
    const float aspect = (float) screenH / screenW;
    GLint first = bufferIndex;

    for (float x = std::floor(graph->getBoundingBox().minX); x <= std::ceil(graph->getBoundingBox().maxX); x += 1) {
        if (x != 0) {
            GLfloat line[] = {
                    x, graph->getBoundingBox().centerY()+ graph->getBoundingBox().height() * aspect, 0, 0.3f, 0.3f, 0.3f, 1,
                    x, graph->getBoundingBox().centerY()- graph->getBoundingBox().height() * aspect, 0, 0.3f, 0.3f, 0.3f, 1
            };
            glBufferSubData(GL_ARRAY_BUFFER, bufferIndex * VERTEX_BYTES, 2 * VERTEX_BYTES, line);
            bufferIndex += 2;
        }
    }

    for (float y = std::floor(graph->getBoundingBox().centerY() - graph->getBoundingBox().height() * aspect); y <= std::ceil(graph->getBoundingBox().centerY() + graph->getBoundingBox().height() * aspect); y += 1) {
        if (y != 0) {
            GLfloat line[] = {
                    graph->getBoundingBox().minX, y, 0, 0.3f, 0.3f, 0.3f, 1,
                    graph->getBoundingBox().maxX, y, 0, 0.3f, 0.3f, 0.3f, 1
            };
            glBufferSubData(GL_ARRAY_BUFFER, bufferIndex * VERTEX_BYTES, 2 * VERTEX_BYTES, line);
            bufferIndex += 2;
        }
    }

    glLineWidth(1);
    glDrawArrays(GL_LINES, first, bufferIndex - first);

    GLfloat axes[] = {
            0, graph->getBoundingBox().centerY() - graph->getBoundingBox().height() * aspect, 0, 0.6f, 0.6f, 0.6f, 1,
            0, graph->getBoundingBox().centerY() + graph->getBoundingBox().height() * aspect, 0, 0.6f, 0.6f, 0.6f, 1,
            graph->getBoundingBox().minX, 0, 0, 0.6f, 0.6f, 0.6f, 1,
            graph->getBoundingBox().maxX, 0, 0, 0.6f, 0.6f, 0.6f, 1,
    };
    glBufferSubData(GL_ARRAY_BUFFER, bufferIndex * VERTEX_BYTES, 4 * VERTEX_BYTES, axes);
    glLineWidth(2);
    glDrawArrays(GL_LINES, bufferIndex, 4);
    bufferIndex += 4;
}


void GraphView::drawElements() {

    float precision = 3 * graph->getBoundingBox().width() / screenW;

    graph->calculateVertices(precision); // To be moved to another thread

    int* segmentIndices; // See Graph::getVertices() for more information
    int numSegs;
    GLfloat* vertices = graph->getVertices(segmentIndices, numSegs);

    glBufferSubData(GL_ARRAY_BUFFER, bufferIndex * VERTEX_BYTES, segmentIndices[numSegs-1]*VERTEX_BYTES, vertices);

    glLineWidth(2.5f);

    int count, lastIndex = 0;
    for (int i = 0; i < numSegs; i++) {
        count = segmentIndices[i]-lastIndex;
        lastIndex = segmentIndices[i];
        glDrawArrays(GL_LINE_STRIP, bufferIndex, count);
        bufferIndex += count;
    }


    /*
    auto graphFunc = [](float x) -> float {
        return 4.0f * sinf(2.0f * x) + 2.5f * cosf(1.3f * x);
    };

    GLint first = bufferIndex;
    float spacing = 3 * (graphR - graphL) / screenW;

    for (float x = std::floor(graphL); x <= std::ceil(graphR) + spacing; x += spacing) {
        GLfloat vertex[] = {
                x, graphFunc(x), 0, 0.8f, 0.2f, 0.1f, 1
        };
        glBufferSubData(GL_ARRAY_BUFFER, bufferIndex * VERTEX_BYTES, VERTEX_BYTES, vertex);
        ++bufferIndex;
    }

    glLineWidth(2.5f);
    glDrawArrays(GL_LINE_STRIP, first, bufferIndex - first);
    */
}


void GraphView::wheelEvent(QWheelEvent* event) {
    QPoint pixelScroll = event->pixelDelta();
    QPoint angleScroll = event->angleDelta() / 8;

    if (!pixelScroll.isNull()) {
        zoom((float) pixelScroll.y() / 50); // TODO: Needs testing with high-resolution scroll
    } else if (!angleScroll.isNull()) {
        // Usually 15 degrees per step
        zoom((float) angleScroll.y() / 15);
    }

    event->accept();
}


void GraphView::zoom(float steps) {
    const float sensitivity = 0.1f;
    BoundingBox bounds = graph->getBoundingBox();

    float z = 1 - sensitivity * steps;

    BoundingBox newBounds = {
            bounds.centerX() - bounds.width() * 0.5f * z,
            bounds.centerX() + bounds.width() * 0.5f * z,
            bounds.centerY() - bounds.height() * 0.5f * z,
            bounds.centerY() + bounds.height() * 0.5f * z
    };
    graph->setBoundingBox(newBounds);

    adjustCamera();
    update();
}


void GraphView::adjustCamera() {
    const float aspect = (float) screenH / screenW;

    projection.setToIdentity();
    projection.ortho(
            -0.5f * graph->getBoundingBox().width(),
            0.5f * graph->getBoundingBox().width(),
            -0.5f * graph->getBoundingBox().height() * aspect,
            0.5f * graph->getBoundingBox().height() * aspect,
            0, 100);

    QVector3D viewEye(graph->getBoundingBox().centerX(), graph->getBoundingBox().centerY(), 20);
    QVector3D viewCenter(graph->getBoundingBox().centerX(), graph->getBoundingBox().centerY(), -1);
    QVector3D viewUp(0, 1, 0);

    QMatrix4x4 view;
    view.lookAt(viewEye, viewCenter, viewUp);

    projection *= view;
}


GLuint GraphView::createShader(const char* vertexSource, const char* fragmentSource, int attribCount, const char* attribs[]) {
    initializeOpenGLFunctions();

    // Create shader objects
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint status = GL_FALSE;
    int infoLogLength;

    // Compile vertex shader
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char infoLog[infoLogLength];
        glGetShaderInfoLog(vertexShader, infoLogLength, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }

    // Compile fragment shader
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char infoLog[infoLogLength];
        glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }

    // Create and link program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    for (int i = 0; i < attribCount; ++i) {
        glBindAttribLocation(program, i, attribs[i]);
    }

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        char infoLog[infoLogLength];
        glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }

    // Clean up after ourselves
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}