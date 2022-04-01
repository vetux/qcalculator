/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "addonmanager.hpp"

#include "addonhelper.hpp"

std::set<std::string> loadedModules;

void AddonManager::setActiveAddons(const std::set<std::string> &addons, AddonManagerListener &listener) {
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
