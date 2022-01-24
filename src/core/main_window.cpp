#include <QtWidgets>

#include "main_window.h"
#include "../equations/function.h"


MainWindow::MainWindow() :
        graphView(new GraphView(this,new Graph())),
        equationDock(new QDockWidget(tr("equations"), this)),
        equationList(new QListWidget(equationDock)) {
    setMinimumSize(800, 600);
    setWindowTitle("[*]Untitled Graph - Cubiq Grapher");
    setCentralWidget(graphView);
    createGraphView();
    createEquationList();
    setWindowModified(false);
}


void MainWindow::onModification() {
    setWindowModified(true);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    event->accept();
}


void MainWindow::createGraphView() {
    QSurfaceFormat format;
    format.setSamples(4);
    graphView->setFormat(format);

    Graph* graph = graphView->getGraph();
    auto graphFunc = [](float x) -> float {
        return 4.0f * sinf(x * 2.0f) + cosf(x);
    };

    Equation* func = new Function(Function::IndependentVariable::X, graphFunc);
    graph->addEquation(func);
}


void MainWindow::createEquationList() {
    equationDock->setWidget(equationList);
    equationList->addItem(QString("equations go here"));
    addDockWidget(Qt::LeftDockWidgetArea, equationDock);
}