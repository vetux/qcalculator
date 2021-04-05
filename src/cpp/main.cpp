#include "gui/mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName("Xenotux");
    a.setApplicationName("qcalc");
    a.setApplicationDisplayName("QCalculator");
    a.setApplicationVersion("v0.3.2");

    MainWindow w;
    w.show();

    return a.exec();
}
