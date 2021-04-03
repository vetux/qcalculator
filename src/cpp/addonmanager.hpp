#ifndef QCALC_ADDONMANAGER_HPP
#define QCALC_ADDONMANAGER_HPP

#include <set>
#include <string>
#include <functional>

class AddonManager {
public:
    /**
     * This function ensures that the addon modules in the passed set are currently loaded,
     * and unloads any modules which are not contained in the set.
     *
     * @param addons The set of module names which should currently be loaded.
     * @param errorCallback The callback to invoke when a addon fails to load / unload.
     * The first argument is the module name and the second argument the error message.
     */
    void setActiveAddons(const std::set<std::string> &addons,
                         const std::function<void(std::string, std::string)>& errorCallback);

    /**
     * @return The set of currently loaded modules.
     */
    std::set<std::string> getActiveAddons();

private:
    std::set<std::string> loadedModules; //The collection of module names which are currently loaded.
};

#endif //QCALC_ADDONMANAGER_HPP
