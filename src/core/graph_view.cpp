#include "graph_view.h"

#include <cmath>
#include <iostream>
#include <QPainter>
#include <QMouseEvent>


namespace Cubiq {

    const int GraphView::BATCH_SIZE = 5000;

    GraphView::GraphView(QWidget* parent, Graph* g) :
            QWidget(parent),
            calculationThread(this, g), graph(g),
            dragging(false), dragStartBounds{0, 0, 0, 0},
            background(34, 34, 34),
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

        adjustTransform();
        calculationThread.markToUpdate();
        update();
    }

    Graph* GraphView::getGraph() {
        return graph;
    }


    void GraphView::centerOrigin() {
        BoundingBox bb = graph->getBoundingBox();
        graph->setBoundingBox(bb.moved(-bb.centerX(), -bb.centerY()));

        adjustTransform();
        calculationThread.markToUpdate();
        update();
    }


    void GraphView::drawGrid(QPainter& painter) {
        const float aspect = (float) height() / (float) width();
        const BoundingBox& bounds = graph->getBoundingBox();

        QPen minorPen(QColor::fromRgbF(1, 1, 1, 0.05));
        minorPen.setWidthF(1);
        minorPen.setCapStyle(Qt::FlatCap);
        QPen majorPen(QColor::fromRgbF(1, 1, 1, 0.2));
        majorPen.setWidthF(1);
        majorPen.setCapStyle(Qt::FlatCap);
        QPen axisPen(QColor::fromRgbF(1, 1, 1, 0.5));
        axisPen.setWidthF(2);
        axisPen.setCapStyle(Qt::FlatCap);

        for (float x = std::floor(bounds.minX / gridSpaceX) * gridSpaceX - gridSpaceX;
                x <= std::ceil(bounds.maxX / gridSpaceX) * gridSpaceX + gridSpaceX;
                x += gridSpaceX) {
            int n = (int) std::round(x / gridSpaceX);
            painter.setPen(n == 0 ? axisPen : (n % gridMajorX == 0 ? majorPen : minorPen));
            painter.drawLine(transform.map(QPointF(x, bounds.centerY() - 0.5f * bounds.height() * aspect - gridSpaceY)),
                             transform.map(QPointF(x, bounds.centerY() + 0.5f * bounds.height() * aspect + gridSpaceY)));
        }

        for (float y = std::floor((bounds.centerY() - 0.5f * bounds.height() * aspect) / gridSpaceY) * gridSpaceY - gridSpaceY;
                y <= std::ceil((bounds.centerY() + 0.5f * bounds.height() * aspect) / gridSpaceY) * gridSpaceY + gridSpaceY;
                y += gridSpaceY) {
            int n = (int) std::round(y / gridSpaceY);
            painter.setPen(n == 0 ? axisPen : (n % gridMajorY == 0 ? majorPen : minorPen));
            painter.drawLine(transform.map(QPointF(bounds.minX - gridSpaceX, y)),
                             transform.map(QPointF(bounds.maxX + gridSpaceX, y)));
        }
    }


    void GraphView::drawElements(QPainter& painter) {
        // Lock the graph's mutex while drawing
        std::scoped_lock<std::mutex> lock(graph->getMutex());

        unsigned long numVerts;
        int count;
        float* vertices = graph->getVertexList(numVerts);

        if (numVerts == 0) return;

        QPen pen(QColor::fromRgbF(0.8, 0.3, 0.3, 0.9));
        pen.setWidthF(2.5);
        painter.setPen(pen);

        const int batchSize = BATCH_SIZE - (BATCH_SIZE % 2);
        QPointF points[batchSize];

        while (numVerts > 0) {
            count = (int) std::fminl(batchSize, numVerts);
            for (int i = 0; i < count; ++i) {
                points[i] = transform.map(QPointF(vertices[7 * i], vertices[7 * i + 1]));
            }
            painter.drawPolyline(points, count);

            vertices += 7 * count;
            numVerts -= count;
        }
    }


    void GraphView::paintEvent(QPaintEvent* event) {
        QPainter painter(this);

        // Fill background
        painter.fillRect(rect(), background);

        painter.setRenderHint(QPainter::Antialiasing, true);

        // Draw grid and graph elements
        drawGrid(painter);
        drawElements(painter);
    }


    void GraphView::resizeEvent(QResizeEvent* event) {
        setMinimumWidth(height() / 8);

        adjustTransform();
        calculationThread.markToUpdate();
        update();
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
        if (!dragging) return;

        const float aspect = (float) height() / (float) width();

        graph->setBoundingBox(dragStartBounds.moved(
                dragStartBounds.width() * (float) (dragStartPos.x() - event->x()) / (float) width(),
                dragStartBounds.height() * aspect * (float) (event->y() - dragStartPos.y()) / (float) height()));

        adjustTransform();
        calculationThread.markToUpdate();
        update();
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

        const float aspect = (float) height() / (float) width();
        float posX = bounds.minX + ((float) pos.x() / (float) width()) * bounds.width();
        float posY = bounds.minY + (0.5 - (((float) pos.y() / (float) height()) - 0.5) * aspect) * bounds.height();

        float z = std::fmaxf(1 - sensitivity * steps, sensitivity);

        BoundingBox newBounds = {
                std::fminf(posX - (posX - bounds.minX) * z, posX - 0.00001f),
                std::fmaxf(posX + (bounds.maxX - posX) * z, posX + 0.00001f),
                std::fminf(posY - (posY - bounds.minY) * z, posY - 0.00001f),
                std::fmaxf(posY + (bounds.maxY - posY) * z, posY + 0.00001f)
        };
        graph->setBoundingBox(newBounds);

        adjustTransform();
        calculationThread.markToUpdate();
        update();
    }


    void GraphView::adjustTransform() {
        const float aspect = (float) height() / (float) width();

        transform.reset();
        transform.translate(0.5f * (float) width(), 0.5f * (float) height());
        transform.scale((float) width() / graph->getBoundingBox().width(), -(float) height() / (graph->getBoundingBox().height() * aspect));
        transform.translate(-graph->getBoundingBox().centerX(), -graph->getBoundingBox().centerY());

        // Adjust grid spacing
        while (gridSpaceX * (float) width() / graph->getBoundingBox().width() > 50.0f) {
            gridStepDown(gridSpaceX, gridMajorX);
        }
        while (gridSpaceX * (float) width() / graph->getBoundingBox().width() < 20.0f) {
            gridStepUp(gridSpaceX, gridMajorX);
        }
        while (gridSpaceY * (float) height() / (graph->getBoundingBox().height() * aspect) > 50.0f) {
            gridStepDown(gridSpaceY, gridMajorY);
        }
        while (gridSpaceY * (float) height() / (graph->getBoundingBox().height() * aspect) < 20.0f) {
            gridStepUp(gridSpaceY, gridMajorY);
        }
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
                precision = 3 * (double) graph->getBoundingBox().width() / (double) parent->width();
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