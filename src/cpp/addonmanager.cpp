#include "addonmanager.hpp"

#include "addonhelper.hpp"

AddonManager::AddonManager(AddonManagerListener &listener) : listener(listener) {}

void AddonManager::setActiveAddons(const std::set<std::string> &addons) {
    for (auto &module : loadedModules) {
        if (addons.find(module) == addons.end()) {
            try {
                AddonHelper::unload(module);
            }
            catch (const std::exception &e) {
                listener.onAddonUnloadFail(module, e.what());
            }
        }
    }
    
    for (auto &module : addons) {
        if (loadedModules.find(module) == loadedModules.end()) {
            try {
                AddonHelper::load(module);
            }
            catch (const std::exception &e) {
                listener.onAddonLoadFail(module, e.what());
            }
        }
    }
    loadedModules = addons;
}

std::set<std::string> AddonManager::getActiveAddons() {
    return loadedModules;
}
