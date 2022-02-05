#pragma once

#include <QMainWindow>
#include <QListWidget>

#include "core/graph_view.h"


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    GraphView* graphView;
    QDockWidget* equationDock;
    QListWidget* equationList;

    QAction* createAction(const char* name, const char* text, const char* slot, const char* shortcut, const char* whatsThis);

    void createGraphView();
    void createEquationList();
    void createTopBar();

public slots:
    void handleNew();
    void handleOpen();
    bool handleSave();
    bool handleSaveAs();
    void handleSettings();

    void handleCopy();
    void handleCut();

};
