#ifndef QCALC_SETTINGS_HPP
#define QCALC_SETTINGS_HPP

#include <QDockWidget>
#include <QSize>

#include <set>
#include <string>

struct Settings {
    bool showKeypad = false;
    bool showBitView = false;

    int historyLimit = 0;

    int activeTab = 0;

    QSize windowSize = {250, 150};

    std::set<std::string> enabledAddonModules; // A set of module names which have been enabled by the user.

    int settingsTab = 0;

    bool showAddonWarning = true;
};

#endif //QCALC_SETTINGS_HPP
