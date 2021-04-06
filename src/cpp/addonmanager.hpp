#ifndef QCALC_ADDONMANAGER_HPP
#define QCALC_ADDONMANAGER_HPP

#include <set>
#include <string>
#include <functional>

#include "addonmanagerlistener.hpp"

namespace AddonManager {
    /**
     * This function ensures that the addon modules in the passed set are currently loaded,
     * and unloads any modules which are not contained in the set.
     *
     * If an addon fails to load / unload it is still considered "loaded".
     *
     * @param addons The set of module names which should currently be loaded.
     * @param listener The listener instance to invoke callbacks on in case of errors.
     */
    void setActiveAddons(const std::set<std::string> &addons, AddonManagerListener &listener);

    /**
     * @return The set of currently loaded modules.
     */
    std::set<std::string> getActiveAddons();
}

#endif //QCALC_ADDONMANAGER_HPP
