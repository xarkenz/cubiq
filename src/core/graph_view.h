#pragma once

#include <QWidget>
#include <thread>

#include "core/graph.h"


namespace Cubiq {

    class GraphView : public QWidget {

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
        void drawGrid(QPainter& painter);
        void drawElements(QPainter& painter);

        void paintEvent(QPaintEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;

        void zoom(float steps, QPointF pos);
        void adjustTransform();

        static void gridStepUp(float& space, int& major);
        static void gridStepDown(float& space, int& major);

    private:
        QColor background;

        Graph* graph;
        CalculationThread calculationThread;

        QTransform transform;

        QPoint dragStartPos;
        BoundingBox dragStartBounds;
        bool dragging;

        float gridSpaceX, gridSpaceY;
        int gridMajorX, gridMajorY;

    };

}