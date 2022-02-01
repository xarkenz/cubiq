#include <QApplication>
#include <QCommandLineParser>
#include <QPushButton>

#include "main_window.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Cubiq");
    QCoreApplication::setApplicationName("Cubiq Grapher");
    QApplication::setWindowIcon(QIcon(":/assets/icons/cubiq_bg.svg"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.process(app);

    MainWindow mainWin;
    mainWin.showMaximized();

    return QApplication::exec();
}