#include <QtWidgets>
#include <QtSvg>
#include <iostream>

#include "main_window.h"
#include "../equations/function.h"


namespace Cubiq {

    MainWindow::MainWindow() :
            graphView(new GraphView(this, new Graph())),
            equationDock(new QDockWidget(tr("Equations"), this)),
            equationList(new QListWidget(equationDock)) {
        setMinimumSize(800, 600);
        setCentralWidget(graphView);
        setWindowModified(false);
        createTopBar();
        createGraphView();
        createEquationList();
        updateInfo();
    }


    QAction* MainWindow::createAction(const char* name, const char* text, const char* slot, const char* shortcut, const char* toolTip) {
        std::string iconFile(":/icons/");
        iconFile += name;
        auto* action = new QAction(QIcon(iconFile.c_str()), tr(text), this);
        action->setShortcut(tr(shortcut));
        action->setToolTip(toolTip);
        connect(action, SIGNAL(triggered()), this, slot);
        return action;
    }


    void MainWindow::createGraphView() {
        // Enable multisampling
        QSurfaceFormat format;
        format.setSamples(4);
        graphView->setFormat(format);

        Graph* graph = graphView->getGraph();

        auto testFunc1 = [](float x) -> float { return 2.0f * sinf(x * 3.0f) + 2.0f * cosf(x * 1.3f); };
        Equation* eq1 = new Function({0.8f, 0.3f, 0.3f, 0.9f}, Function::IndependentVariable::X, testFunc1);
        graph->addEquation(eq1);

        /*auto testFunc2 = [](float y) -> float {return y*y*y*y/16 - y*y*y*5/8 + y*y + y*2;};
        Equation* eq2 = new Function({0.2f,0.2f,0.8f,0.9f}, Function::IndependentVariable::Y, testFunc2);
        graph->addEquation(eq2);*/

        /*auto testFunc3 = [](float x) -> float {return tanf(x);};
        Equation* eq3 = new Function({0.2f,0.8f,0.2f,0.9f}, Function::IndependentVariable::X, testFunc3);
        graph->addEquation(eq3);*/

        /*auto testFunc4 = [](float x, float y) -> float {return x*x/4 + y*y/2 + (x+2)*y/3 - 5;};
        Equation* eq4 = new ImplicitEquation({0.8f,0.8f,0.2f,0.9f}, testFunc4);
        graph->addEquation(eq4);*/

        /*auto testFunc5 = [](float x, float y) -> float {return x*y;};
        Equation* eq5 = new ImplicitEquation({0.8f,0.2f,0.8f,0.9f}, testFunc5);
        graph->addEquation(eq5);*/

        /*auto disgustingFunc1 = [](float x, float y) -> float {return tan(pow(x, y)) - sin(pow(x, cos(y)));};
        Equation* eqg1 = new ImplicitEquation({0.8f,0.8f,0.2f,0.9f}, disgustingFunc1);
        graph->addEquation(eqg1);*/

        /*auto disgustingFunc2 = [](float x, float y) -> float {return tan(x*x + y*y) - 1;};
        Equation* eqg2 = new ImplicitEquation({0.8f,0.8f,0.2f,0.9f}, disgustingFunc2);
        graph->addEquation(eqg2);*/

        setWindowModified(true);
    }


    void MainWindow::createEquationList() {
        // TODO: why does animated svg not work :(
        /*auto* loadingIcon = new QSvgWidget(":/icons/loading", equationDock);
        equationDock->setWidget(loadingIcon);
        std::cout << loadingIcon->renderer()->animated() << std::endl;
        loadingIcon->setMaximumSize(64, 64);
        loadingIcon->renderer()->setFramesPerSecond(30);*/
        equationDock->setFeatures(QDockWidget::DockWidgetMovable);
        equationDock->setWidget(equationList);
        equationList->setWordWrap(true);
        equationList->addItem(tr("There's nothing here yet. Divert your eyes."));
        addDockWidget(Qt::LeftDockWidgetArea, equationDock);
    }


    void MainWindow::createTopBar() {
        // Define all menu actions
        QAction* aNew = createAction("new", "New", SLOT(handleNew()), "Ctrl+N", "Create a new graph.");
        QAction* aOpen = createAction("open", "Open...", SLOT(handleOpen()), "Ctrl+O", "Open a graph from file.");
        QAction* aSave = createAction("save", "Save", SLOT(handleSave()), "Ctrl+S", "Save the current graph.");
        QAction* aSaveAs = createAction("saveas", "Save as...", SLOT(handleSaveAs()), "Ctrl+Shift+S", "Save the graph under a new name.");
        QAction* aSettings = createAction("settings", "Settings...", SLOT(handleSettings()), "Ctrl+Alt+S", "Configure the look and feel of the program.");

        QAction* aCopy = createAction("copy", "Copy", SLOT(handleCopy()), "Ctrl+C", "Copy the current selection to clipboard.");
        QAction* aCut = createAction("cut", "Cut", SLOT(handleCut()), "Ctrl+X", "Cut the current selection to clipboard.");

        QAction* aOrigin = createAction("origin", "Return to Origin", SLOT(handleOrigin()), "Ctrl+.", "Center the view on (0, 0).");

        QAction* aAbout = createAction("about", "About Cubiq...", SLOT(handleAbout()), "", "More information about this program.");

        // Create menus and add respective actions
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
        mView->addAction(aOrigin);

        QMenu* mWindow = menuBar()->addMenu("Window");

        QMenu* mHelp = menuBar()->addMenu("Help");
        mHelp->addAction(aAbout);

        // Create quick action toolbar
        QToolBar* toolBar = addToolBar("Quick Actions");
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        toolBar->setAllowedAreas(Qt::TopToolBarArea);

        toolBar->addAction(aNew);
        toolBar->addAction(aOpen);
        toolBar->addAction(aSave);
        toolBar->addAction(aSaveAs);
        toolBar->addAction(aSettings);
        toolBar->addSeparator();
        toolBar->addAction(aCopy);
        toolBar->addAction(aCut);
        toolBar->addSeparator();
        toolBar->addAction(aOrigin);

        // Add a corner icon to the toolbar
        auto* spacer = new QWidget;
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        toolBar->addWidget(spacer);
        toolBar->addAction(aAbout);
        toolBar->widgetForAction(aAbout)->setStyleSheet("margin-right: 6px;");
    }


    bool MainWindow::checkUnsavedChanges() {
        if (isWindowModified()) {
            QMessageBox dialog;
            dialog.setIcon(QMessageBox::Question);
            dialog.setText("This graph has unsaved changes.");
            dialog.setInformativeText("Would you like to save them?");
            dialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            dialog.setDefaultButton(QMessageBox::Save);

            int option = dialog.exec();
            switch (option) {
                case QMessageBox::Save:
                    return handleSave();
                case QMessageBox::Discard:
                    setWindowModified(false);
                    return true;
                default:
                    return false;
            }
        }
        return true;
    }


    void MainWindow::closeEvent(QCloseEvent* event) {
        if (checkUnsavedChanges()) {
            event->accept();
        } else {
            event->ignore();
        }
    }


    void MainWindow::handleNew() {
        if (checkUnsavedChanges()) {
            graphView->setGraph(new Graph());
            updateInfo();
        }
    }

    void MainWindow::handleOpen() {
        // Open file
    }

    bool MainWindow::handleSave() {
        // Handle saving: true if saved successfully, false if cancelled (i.e. exited Save As)
        setWindowModified(false);
        return true;
    }

    bool MainWindow::handleSaveAs() {
        // Open file dialog to save under a filename, true if saved, false if cancelled
        setWindowModified(false);
        return true;
    }

    void MainWindow::handleSettings() {
        SettingsDialog dialog;
        dialog.exec();
    }

    void MainWindow::handleCopy() {
        // Copy
    }

    void MainWindow::handleCut() {
        // Cut
    }

    void MainWindow::handleOrigin() {
        graphView->centerOrigin();
    }

    void MainWindow::handleAbout() {
        // About
    }


    void MainWindow::updateInfo() {
        QString title("[*] - Cubiq Grapher");
        setWindowTitle(title.insert(3, graphView->getGraph()->getName()));
        update();
    }

}