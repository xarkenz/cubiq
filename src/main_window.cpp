#include <QtWidgets>
#include <Qt>

#include "main_window.h"


MainWindow::MainWindow() :
        graphView(new GraphView(this)),
        equationDock(new QDockWidget(tr("Equations"), this)),
        equationList(new QListWidget(equationDock)) {
    showMaximized();
    setMinimumSize(400, 300);
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

}


void MainWindow::createEquationList() {
    equationDock->setWidget(equationList);
    equationList->addItem(QString("equations go here"));
    addDockWidget(Qt::LeftDockWidgetArea, equationDock);
}