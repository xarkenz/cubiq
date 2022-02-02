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

    QAction* createAction(const char* name, const char* text, const char* shortcut);

    void createGraphView();
    void createEquationList();
    void createTopBar();

};
