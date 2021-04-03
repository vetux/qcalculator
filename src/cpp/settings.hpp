#ifndef QCALC_SETTINGS_HPP
#define QCALC_SETTINGS_HPP

#include <QDockWidget>
#include <QSize>

#include <set>
#include <string>

struct Settings {
    bool showKeypad = false;
    bool showBitView = false;
    bool showDock = false;

    int historyLimit = 0;

    Qt::DockWidgetArea dockPosition = Qt::DockWidgetArea::BottomDockWidgetArea;
    int dockActiveTab = 0;

    QSize windowSize = {};

    std::set<std::string> enabledAddonModules; // A set of module names which have been enabled by the user.

    int settingsTab = 0;
};

#endif //QCALC_SETTINGS_HPP
