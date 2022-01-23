#include "graph_view.h"

#include <iostream>


const char* BASIC_VERTEX =
#include "shader/basic_vertex_glsl.h"

const char* BASIC_FRAGMENT =
#include "shader/basic_fragment_glsl.h"


GraphView::GraphView(QWidget* parent) :
            QOpenGLWidget(parent) {
        clearR = .1f;
        clearG = .1f;
        clearB = .1f;

        screenW = 0;
        screenH = 0;

        graphL = -10;
        graphR = 10;
        graphB = -10;
        graphT = 10;
}


void GraphView::initializeGL() {
    initializeOpenGLFunctions();

    // Configure alpha blending
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

    adjustCamera();
}


void GraphView::paintGL() {
    initializeOpenGLFunctions();

    // Temporary
    GLfloat vertices[] = {
            -50, -50, 0, 1, 0, 0, 1,
            50, -50, 0, 0, 1, 0, 1,
            0,  50, 0, 0, 0, 1, 1
    };

    // Prepare the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Upload projection matrix to shader
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, projection.data());

    // Prepare VAO/VBO
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Temporary
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Unbind VAO/VBO, stop using shader
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}


void GraphView::adjustCamera() {
    projection.setToIdentity();
    projection.ortho(-.5f * screenW, .5f * screenW, -.5f * screenH, .5f * screenH, 0, 100);

    QVector3D viewEye(.5f * (graphL + graphR), .5f * (graphB + graphT), 20);
    QVector3D viewCenter(.5f * (graphL + graphR), .5f * (graphB + graphT), -1);
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
