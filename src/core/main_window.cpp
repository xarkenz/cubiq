#include <QtWidgets>

#include "main_window.h"
#include "../equations/function.h"
#include "equations/implicit_equation.h"


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


    auto testFunc1 = [](float x) -> float {return 4.0f * sinf(x * 2.0f) + cosf(x);};
    Equation::DisplaySettings ds1 {0.8f,0.2f,0.2f,1.0f};
    Equation* eq1 = new Function(ds1, Function::IndependentVariable::X, testFunc1);
    graph->addEquation(eq1);

    auto testFunc2 = [](float y) -> float {return y*y*y*y/16 - y*y*y*5/8 + y*y + y*2;};
    Equation::DisplaySettings ds2 {0.2f,0.2f,0.8f,1.0f};
    Equation* eq2 = new Function(ds2, Function::IndependentVariable::Y, testFunc2);
    graph->addEquation(eq2);

    auto testFunc3 = [](float x) -> float {return tanf(x);};
    Equation::DisplaySettings ds3 {0.2f,0.8f,0.2f,1.0f};
    Equation* eq3 = new Function(ds3, Function::IndependentVariable::X, testFunc3);
    graph->addEquation(eq3);

    auto testFunc4 = [](float x, float y) -> float {return x*x/4 + y*y/2 + (x+2)*y/3 - 5;};
    Equation::DisplaySettings ds4 {0.8f,0.8f,0.2f,1.0f};
    Equation* eq4 = new ImplicitEquation(ds4, testFunc4);
    graph->addEquation(eq4);

}


void MainWindow::createEquationList() {
    equationDock->setWidget(equationList);
    equationList->addItem(QString("equations go here"));
    addDockWidget(Qt::LeftDockWidgetArea, equationDock);
}