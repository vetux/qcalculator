#ifndef QCALC_SETTINGS_HPP
#define QCALC_SETTINGS_HPP

#include <QDockWidget>
#include <QSize>

#include <set>
#include <string>
#include <map>

/**
 * The settings object should store arbitrary data identified by key value pairs.
 *
 * Addons will get this exposed so that addons can define persistent state which will all be saved in one batch
 * when the main window exits.
 *
 * This way the addons dont have to care about how, when and where the settings are stored.
 */
class Settings {
public:

};

#endif //QCALC_SETTINGS_HPP
