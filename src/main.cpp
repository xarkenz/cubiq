#include <QApplication>
#include <QCommandLineParser>

#include "core/main_window.h"

#include "style/dark_qss.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Cubiq");
    QCoreApplication::setApplicationName("Grapher");
    QApplication::setWindowIcon(QIcon(":/icons/cubiq_bg"));

    app.setStyleSheet(DARK_QSS);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.process(app);

    Cubiq::MainWindow mainWin;
    mainWin.showMaximized();

    return QApplication::exec();
}