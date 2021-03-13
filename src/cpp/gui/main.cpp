#include "gui/mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationDisplayName("Qt-Calculator");
    a.setApplicationVersion("v0.0.3");

    MainWindow w;
    w.show();

    return a.exec();
}
