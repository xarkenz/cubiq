#pragma once

#include <QMainWindow>
#include <QListWidget>

#include "core/graph_view.h"


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();

    void onModification();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    GraphView* graphView;
    QDockWidget* equationDock;
    QListWidget* equationList;

    void createGraphView();

    void createEquationList();

};
