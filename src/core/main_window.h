#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>

#include "core/graph_view.h"
#include "core/settings_dialog.h"


namespace Cubiq {

    class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        MainWindow();

        void markToUpdate();

    private:
        GraphView* graphView;
        QDockWidget* equationDock;
        QListWidget* equationList;

        QAction*
        createAction(const char* name, const char* text, const char* slot, const char* shortcut, const char* toolTip);

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

    // TODO: move this
    class EquationInputLine : public QLineEdit {
        Q_OBJECT
        public:
            EquationInputLine(MainWindow* mainWin, Equation** equationPtr, const QString &contents, QWidget *parent = nullptr);
            EquationInputLine(MainWindow* mainWin, Equation** equationPtr, QWidget *parent = nullptr);

        private:
            MainWindow* mainParent;
            Equation** equationPointer;

            void focusOutEvent(QFocusEvent *e) override;

        public slots:
            void onChange(const QString &text);
    };

}
