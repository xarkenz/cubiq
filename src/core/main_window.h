#pragma once

#include <QMainWindow>
#include <QListWidget>

#include "core/graph_view.h"
#include "core/settings_dialog.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    GraphView* graphView;
    QDockWidget* equationDock;
    QListWidget* equationList;

    QAction* createAction(const char* name, const char* text, const char* slot, const char* shortcut, const char* whatsThis);

    void createGraphView();
    void createEquationList();
    void createTopBar();

    bool checkUnsavedChanges();

protected:
    void closeEvent(QCloseEvent* event) override;

public slots:
    void handleNew();
    void handleOpen();
    bool handleSave();
    bool handleSaveAs();
    void handleSettings();

    void handleCopy();
    void handleCut();

    void handleOrigin();

    void updateInfo();

};
