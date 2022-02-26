#include "graph_view.h"

#include <cmath>
#include <iostream>

#include <QWheelEvent>


namespace Cubiq {

    #include "shader/basic_vertex_glsl.h"
    #include "shader/basic_fragment_glsl.h"


    const int GraphView::BATCH_SIZE = 5000;

    const GLsizei VERTEX_BYTES = 7 * sizeof(GLfloat);


    GraphView::GraphView(QWidget* parent, Graph* g) :
            QOpenGLWidget(parent),
            calculationThread(this, g), graph(g),
            dragging(false), dragStartBounds{0, 0, 0, 0},
            clearR(0.133f), clearG(0.133f), clearB(0.133f),
            screenW(0), screenH(0),
            gridSpaceX(1), gridSpaceY(1), gridMajorX(5), gridMajorY(5) {
    }

    GraphView::~GraphView() {
        calculationThread.markToExit();
        calculationThread.join();

        delete graph;
    }


    void GraphView::setGraph(Graph* g) {
        delete graph;
        graph = g;
        calculationThread.setGraph(g);

        adjustCamera();
        calculationThread.markToUpdate();
        update();
    }

    Graph* GraphView::getGraph() {
        return graph;
    }


    void GraphView::centerOrigin() {
        BoundingBox bb = graph->getBoundingBox();
        graph->setBoundingBox(bb.moved(-bb.centerX(), -bb.centerY()));

        adjustCamera();
        calculationThread.markToUpdate();
        update();
    }


    void GraphView::initializeGL() {
        initializeOpenGLFunctions();

        // Configure rendering
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Configure screen clear color
        glClearColor(clearR, clearG, clearB, 1);

        // Compile and link shader program
        const char* attribs[] = {"vPos", "vColor"};
        shaderProgram = createShader(BASIC_VERTEX_GLSL, BASIC_FRAGMENT_GLSL, 2, attribs);
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
        calculationThread.markToUpdate();
        update();
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
        const float aspect = (float) screenH / (float) screenW;

        const BoundingBox& bounds = graph->getBoundingBox();

        const float minorAlpha = 0.05f;
        const float majorAlpha = 0.2f;
        const float axisAlpha = 0.5f;

        GLfloat axes[] = {
                0, bounds.centerY() - bounds.height() * aspect, 0, 1, 1, 1, axisAlpha,
                0, bounds.centerY() + bounds.height() * aspect, 0, 1, 1, 1, axisAlpha,
                bounds.minX, 0, 0, 1, 1, 1, axisAlpha,
                bounds.maxX, 0, 0, 1, 1, 1, axisAlpha,
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * VERTEX_BYTES, axes);

        int index = 4;

        for (float x = std::floor(bounds.minX / gridSpaceX) * gridSpaceX;
                x <= std::ceil(bounds.maxX / gridSpaceX) * gridSpaceX;
                x += gridSpaceX) {
            if (x != 0) {
                float alpha = (int) std::round(x / gridSpaceX) % gridMajorX == 0 ? majorAlpha : minorAlpha;
                GLfloat line[] = {
                        x, bounds.centerY() - bounds.height() * aspect, 0, 1, 1, 1, alpha,
                        x, bounds.centerY() + bounds.height() * aspect, 0, 1, 1, 1, alpha,
                };
                glBufferSubData(GL_ARRAY_BUFFER, index * VERTEX_BYTES, 2 * VERTEX_BYTES, line);
                index += 2;
            }
        }

        for (float y = std::floor((bounds.centerY() - bounds.height() * aspect) / gridSpaceY) * gridSpaceY;
                y <= std::ceil((bounds.centerY() + bounds.height() * aspect) / gridSpaceY) * gridSpaceY;
                y += gridSpaceY) {
            if (y != 0) {
                float alpha = (int) std::round(y / gridSpaceY) % gridMajorY == 0 ? majorAlpha : minorAlpha;
                GLfloat line[] = {
                        bounds.minX, y, 0, 1, 1, 1, alpha,
                        bounds.maxX, y, 0, 1, 1, 1, alpha,
                };
                glBufferSubData(GL_ARRAY_BUFFER, index * VERTEX_BYTES, 2 * VERTEX_BYTES, line);
                index += 2;
            }
        }

        // Draw regular grid [4, index)
        glLineWidth(1);
        glDrawArrays(GL_LINES, 4, index - 4);

        // Draw grid axes on top [0, 4)
        glLineWidth(2);
        glDrawArrays(GL_LINES, 0, 4);
    }


    void GraphView::drawElements() {
        // Lock the graph's mutex while drawing
        std::scoped_lock<std::mutex> lock(graph->getMutex());

        unsigned long numVerts;
        GLsizei count;
        GLfloat* vertices = graph->getVertexList(numVerts);

        if (numVerts == 0) { return; }

        glLineWidth(2.5f);

        const int batchSize = BATCH_SIZE - (BATCH_SIZE % 2);

        while (numVerts > 0) {
            count = (int) std::fminl(batchSize, numVerts);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * VERTEX_BYTES, vertices);

            glDrawArrays(GL_LINES, 0, count);
            vertices += 7 * count;
            numVerts -= count;
        }

    }


    void GraphView::mousePressEvent(QMouseEvent* event) {
        dragging = true;
        dragStartPos = event->pos();
        dragStartBounds = graph->getBoundingBox();
    }

    void GraphView::mouseReleaseEvent(QMouseEvent* event) {
        dragging = false;
    }

    void GraphView::mouseMoveEvent(QMouseEvent* event) {
        if (dragging) {
            const float aspect = (float) screenH / (float) screenW;

            graph->setBoundingBox(dragStartBounds.moved(
                    dragStartBounds.width() * (float) (dragStartPos.x() - event->x()) / (float) screenW,
                    dragStartBounds.height() * aspect * (float) (event->y() - dragStartPos.y()) / (float) screenH));

            adjustCamera();
            calculationThread.markToUpdate();
            update();
        }
    }

    void GraphView::wheelEvent(QWheelEvent* event) {
        if (dragging) return;

        QPoint pixelScroll = event->pixelDelta();
        QPoint angleScroll = event->angleDelta() / 8;

        if (!pixelScroll.isNull()) {
            zoom((float) pixelScroll.y() / 50, event->pos()); // TODO: pos() is deprecated in favor of position() in newer versions
        } else if (!angleScroll.isNull()) {
            // Usually 15 degrees per step
            zoom((float) angleScroll.y() / 15, event->pos()); // TODO: see above
        }

        event->accept();
    }


    void GraphView::zoom(float steps, QPointF pos) {
        const float sensitivity = 0.1f;
        BoundingBox bounds = graph->getBoundingBox();

        const float aspect = (float) screenH / (float) screenW;
        float posX = bounds.minX + ((float) pos.x() / screenW) * bounds.width();
        float posY = bounds.minY + (0.5 - (((float) pos.y() / screenH) - 0.5) * aspect) * bounds.height();

        float z = std::fmaxf(1 - sensitivity * steps, sensitivity);

        BoundingBox newBounds = {
                std::fminf(posX - (posX - bounds.minX) * z, posX - 0.00001f),
                std::fmaxf(posX + (bounds.maxX - posX) * z, posX + 0.00001f),
                std::fminf(posY - (posY - bounds.minY) * z, posY - 0.00001f),
                std::fmaxf(posY + (bounds.maxY - posY) * z, posY + 0.00001f)
        };
        graph->setBoundingBox(newBounds);

        adjustCamera();
        calculationThread.markToUpdate();
        update();
    }


    void GraphView::adjustCamera() {
        const float aspect = (float) screenH / (float) screenW;

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

        // Adjust grid spacing
        while (gridSpaceX * (float) screenW / graph->getBoundingBox().width() > 50.0f) {
            gridStepDown(gridSpaceX, gridMajorX);
        }
        while (gridSpaceX * (float) screenW / graph->getBoundingBox().width() < 20.0f) {
            gridStepUp(gridSpaceX, gridMajorX);
        }
        while (gridSpaceY * (float) screenH / (graph->getBoundingBox().height() * aspect) > 50.0f) {
            gridStepDown(gridSpaceY, gridMajorY);
        }
        while (gridSpaceY * (float) screenH / (graph->getBoundingBox().height() * aspect) < 20.0f) {
            gridStepUp(gridSpaceY, gridMajorY);
        }
    }


    GLuint GraphView::createShader(const char* vertexSource, const char* fragmentSource, int attribCount, const char* attribs[]) {
        initializeOpenGLFunctions();

        // Determine system GLSL version
        std::string glslVersionStr((char*) glGetString(GL_SHADING_LANGUAGE_VERSION));

        // Convert version string to number compatible with #version directive
        std::string glslVersionID;
        for (char c : glslVersionStr) {
            if (std::isdigit(c)) {
                glslVersionID += c;
            } else if (std::isspace(c)) break;
        }

        // Ensure version does not exceed latest supported version
        if (std::stoi(glslVersionID, nullptr) > 460)
            glslVersionID = "460";

        // Create new source strings with version included
        std::string vertexSourceStr("#version ");
        std::string fragmentSourceStr("#version ");
        vertexSourceStr += glslVersionID + "\n" + vertexSource;
        fragmentSourceStr += glslVersionID + "\n" + fragmentSource;

        // Get C-style string source for each shader
        const char* vertexSourceData = vertexSourceStr.data();
        const char* fragmentSourceData = fragmentSourceStr.data();

        // Create shader objects
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        GLint status = GL_FALSE;
        int infoLogLength;

        // Compile vertex shader
        glShaderSource(vertexShader, 1, &vertexSourceData, NULL);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
            char* infoLog = new char[infoLogLength];
            glGetShaderInfoLog(vertexShader, infoLogLength, NULL, infoLog);
            std::cerr << infoLog << std::endl;
            delete[] infoLog;
        }

        // Compile fragment shader
        glShaderSource(fragmentShader, 1, &fragmentSourceData, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
            char* infoLog = new char[infoLogLength];
            glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, infoLog);
            std::cerr << infoLog << std::endl;
            delete[] infoLog;
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
            char* infoLog = new char[infoLogLength];
            glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
            std::cerr << infoLog << std::endl;
            delete[] infoLog;
        }

        // Clean up after ourselves
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }


    void GraphView::gridStepUp(float& space, int& major) {
        int base = (int) (space / std::pow(10, (int) std::floor(std::log10(space))));
        switch (base) {
            case 1:
            case 5:
                space *= 2.0f;
                major = 5;
                break;
            case 2:
                space *= 2.5f;
                major = 4;
                break;
            default:
                space = 1.0f;
                major = 5;
                break;
        }
    }

    void GraphView::gridStepDown(float& space, int& major) {
        int base = (int) (space / std::pow(10, (int) std::floor(std::log10(space))));
        switch (base) {
            case 1:
                space *= 0.5f;
                major = 4;
                break;
            case 2:
                space *= 0.5f;
                major = 5;
                break;
            case 5:
                space *= 0.4f;
                major = 5;
                break;
            default:
                space = 1.0f;
                major = 5;
                break;
        }
    }


    const int GraphView::CalculationThread::MILLIS_PER_UPDATE = 17;

    GraphView::CalculationThread::CalculationThread(GraphView* p, Graph* g) :
            std::thread(&GraphView::CalculationThread::run, this),
            parent(p),
            graph(g),
            toUpdate(false),
            toExit(false) {

    }

    void GraphView::CalculationThread::run() {
        double precision;
        std::defer_lock_t t;
        while (!toExit) {
            if (toUpdate) {
                precision = 3 * (double) (graph->getBoundingBox().width()) / (double) (parent->screenW);
                graph->calculateVertices(precision);
                parent->update();
                toUpdate = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(MILLIS_PER_UPDATE));
        }
    }

    void GraphView::CalculationThread::markToUpdate() {
        toUpdate = true;
    }

    void GraphView::CalculationThread::markToExit() {
        toExit = true;
    }

    void GraphView::CalculationThread::setGraph(Graph* g) {
        graph = g;
    }

}