#include "addonmanager.hpp"

#include "addonhelper.hpp"

void AddonManager::setActiveAddons(const std::set<std::string> &addons,
                                   const std::function<void(std::string, std::string)>& errorCallback) {
    for (auto &module : loadedModules) {
        if (addons.find(module) == addons.end()) {
            try {
                AddonHelper::unload(module);
            }
            catch (const std::exception &e) {
                errorCallback(module, e.what());
            }
        }
    }
    for (auto &module : addons) {
        if (loadedModules.find(module) == loadedModules.end()) {
            try {
                AddonHelper::load(module);
            }
            catch (const std::exception &e) {
                errorCallback(module, e.what());
            }
        }
    }
    loadedModules = addons;
}

std::set<std::string> AddonManager::getActiveAddons() {
    return loadedModules;
}
