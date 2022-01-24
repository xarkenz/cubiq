#include <QApplication>
#include <QCommandLineParser>
#include <QPushButton>

#include "main_window.h"


int main(int argc, char* argv[]) {
    //Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Cubiq");
    QCoreApplication::setApplicationName("Cubiq Grapher");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.process(app);

    MainWindow mainWin;
    mainWin.showMaximized();

    return QApplication::exec();
}