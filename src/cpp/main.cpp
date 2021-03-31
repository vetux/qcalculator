#include "gui/mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName("xenotux");
    a.setApplicationName("qcalc");
    a.setApplicationDisplayName("QCalculator");
    a.setApplicationVersion("v0.2.0");

    MainWindow w;
    w.show();

    return a.exec();
}
