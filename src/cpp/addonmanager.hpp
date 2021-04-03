#ifndef QCALC_ADDONMANAGER_HPP
#define QCALC_ADDONMANAGER_HPP

#include <set>
#include <string>
#include <functional>

#include "addonmanagerlistener.hpp"

class AddonManager {
public:
    explicit AddonManager(AddonManagerListener &listener);

    /**
     * This function ensures that the addon modules in the passed set are currently loaded,
     * and unloads any modules which are not contained in the set.
     *
     * If an addon fails to load / unload it is still considered "loaded".
     *
     * @param addons The set of module names which should currently be loaded.
     */
    void setActiveAddons(const std::set<std::string> &addons);

    /**
     * @return The set of currently loaded modules.
     */
    std::set<std::string> getActiveAddons();

private:
    AddonManagerListener &listener;
    std::set<std::string> loadedModules; //The collection of module names which are currently loaded.
};

#endif //QCALC_ADDONMANAGER_HPP
