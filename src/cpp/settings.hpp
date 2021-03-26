#ifndef QCALC_SETTINGS_HPP
#define QCALC_SETTINGS_HPP

#include <QDockWidget>
#include <QSize>

struct Settings {
    bool showKeypad = false;
    bool showBitView = false;
    bool showDock = false;

    int historyLimit = 0;

    Qt::DockWidgetArea dockPosition = Qt::DockWidgetArea::BottomDockWidgetArea;
    int dockActiveTab = 0;

    QSize windowSize = {};
};

#endif //QCALC_SETTINGS_HPP
