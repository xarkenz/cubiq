#include <QtWidgets>
#include <iostream>

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
    createTopBar();
    createGraphView();
    createEquationList();
    setWindowModified(false);
}

MainWindow::~MainWindow() {
    delete graphView;
    delete equationList;
    delete equationDock;
}


void MainWindow::onModification() {
    setWindowModified(true);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    event->accept();
}


QAction* MainWindow::createAction(const char* name, const char* text, const char* slot, const char* shortcut, const char* whatsThis) {
    std::string iconFile(":/icons/");
    iconFile += name;
    auto* action = new QAction(QIcon(iconFile.c_str()), tr(text), this);
    action->setShortcut(tr(shortcut));
    action->setWhatsThis(whatsThis);
    connect(action, SIGNAL(triggered()), this, slot);
    return action;
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

    auto testFunc5 = [](float x, float y) -> float {return x*y;};
    Equation::DisplaySettings ds5 {0.8f,0.2f,0.8f,1.0f};
    Equation* eq5 = new ImplicitEquation(ds5, testFunc5);
    graph->addEquation(eq5);

    /*auto disgustingFunc1 = [](float x, float y) -> float {return tan(pow(x, y)) - sin(pow(x, cos(y)));};
    Equation::DisplaySettings dsg1 {0.8f,0.8f,0.2f,1.0f};
    Equation* eqg1 = new ImplicitEquation(dsg1, disgustingFunc1);
    graph->addEquation(eqg1);

    auto disgustingFunc2 = [](float x, float y) -> float {return tan(x*x + y*y) - 1;};
    Equation::DisplaySettings dsg2 {0.8f,0.8f,0.2f,1.0f};
    Equation* eqg2 = new ImplicitEquation(dsg2, disgustingFunc2);
    graph->addEquation(eqg2);*/

}


void MainWindow::createEquationList() {
    equationDock->setWidget(equationList);
    equationList->addItem(QString("equations go here"));
    addDockWidget(Qt::LeftDockWidgetArea, equationDock);
}


void MainWindow::createTopBar() {
    QAction* aNew = createAction("new", "New", SLOT(handleNew()), "Ctrl+N", "Create a new graph.");
    QAction* aOpen = createAction("open", "Open...", SLOT(handleOpen()), "Ctrl+O", "Open a graph from file.");
    QAction* aSave = createAction("save", "Save", SLOT(handleSave()), "Ctrl+S", "Save the current graph.");
    QAction* aSaveAs = createAction("saveas", "Save as...", SLOT(handleSaveAs()), "Ctrl+Shift+S", "Save the graph under a new name.");
    QAction* aSettings = createAction("settings", "Settings...", SLOT(handleSettings()), "Ctrl+Alt+S", "Configure the look and feel of Cubiq.");

    QAction* aCopy = createAction("copy", "Copy", SLOT(handleCopy()), "Ctrl+C", "Copy the current selection to clipboard.");
    QAction* aCut = createAction("cut", "Cut", SLOT(handleCut()), "Ctrl+X", "Cut the current selection to clipboard.");

    QMenu* mFile = menuBar()->addMenu("File");
    mFile->addAction(aNew);
    mFile->addAction(aOpen);
    mFile->addAction(aSave);
    mFile->addAction(aSaveAs);
    mFile->addSeparator();
    mFile->addAction(aSettings);

    QMenu* mEdit = menuBar()->addMenu("Edit");
    mEdit->addAction(aCopy);
    mEdit->addAction(aCut);

    QMenu* mView = menuBar()->addMenu("View");

    QMenu* mWindow = menuBar()->addMenu("Window");

    QMenu* mHelp = menuBar()->addMenu("Help");
}


void MainWindow::handleNew() {
    std::cout << "new" << std::endl;
}

void MainWindow::handleOpen() {
    std::cout << "open" << std::endl;
}

void MainWindow::handleSave() {
    std::cout << "save" << std::endl;
}

void MainWindow::handleSaveAs() {
    std::cout << "save as" << std::endl;
}

void MainWindow::handleSettings() {
    std::cout << "settings" << std::endl;
}

void MainWindow::handleCopy() {
    std::cout << "copy" << std::endl;
}

void MainWindow::handleCut() {
    std::cout << "cut" << std::endl;
}