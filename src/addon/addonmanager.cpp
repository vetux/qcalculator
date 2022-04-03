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

#include <utility>
#include <filesystem>

#include "io/fileoperations.hpp"

#include "cpython/pyutil.hpp"

#include "extern/json.hpp"

struct AddonMetadata {
    std::string displayName;
    std::string description;
};

AddonMetadata deserializeMetadata(const std::string &text) {
    nlohmann::json j = nlohmann::json::parse(text);
    AddonMetadata ret;
    ret.displayName = j["displayName"];
    ret.description = j["description"];
    return ret;
}

static std::map<std::string, AddonMetadata> readAvailableAddons(const std::string &addonDirectory) {
    std::map<std::string, AddonMetadata> ret;

    std::vector<std::string> addonFiles = FileOperations::findFilesInDirectory(addonDirectory, "py");
    for (auto &filePath: addonFiles) {
        std::filesystem::path addonFile(filePath.c_str());

        auto extension = addonFile.extension().string();
        auto moduleName = addonFile.filename().string();

        for (auto c: extension)
            if (!moduleName.empty())
                moduleName.pop_back();

        std::string dirStr(filePath);
        for (auto c: moduleName + extension)
            dirStr.pop_back();

        auto metadataFile = std::filesystem::path(absolute(std::filesystem::path(dirStr)).string()
                                                  + "/"
                                                  + moduleName
                                                  + ".json");

        AddonMetadata metadata;
        if (exists(metadataFile)) {
            //Has metadata file
            try {
                metadata = deserializeMetadata(FileOperations::fileReadAllText(absolute(metadataFile)));
            } catch (const std::exception &e) {
                //Ignore exception and set default metadata
                metadata.displayName = moduleName;
                metadata.description = "No Description";
            }
        } else {
            //No metadata file
            metadata.displayName = moduleName;
            metadata.description = "No Description";
        }

        ret[moduleName] = metadata;
    }

    return ret;
}

static std::atomic<bool> constructFlag; // Only one instance of AddonManager may exist because of cpython api static context.

AddonManager::AddonManager(const std::string &addonDirectory,
                           const std::set<std::string> &moduleDirectories,
                           Listener onAddonLoadFail,
                           Listener onAddonUnloadFail)
        : addonDir(addonDirectory),
          moduleDirs(moduleDirectories),
          onAddonLoadFail(std::move(onAddonLoadFail)),
          onAddonUnloadFail(std::move(onAddonUnloadFail)) {
    PyUtil::initializePython();
    for (auto &p: moduleDirs)
        PyUtil::addModuleDirectory(p);
    readAddons();
}

AddonManager::~AddonManager() {
    setActiveAddons({});
    PyUtil::finalizePython();
}

void AddonManager::reloadModules() {
    std::set<std::string> rAddons;

    // Reload loaded addon modules
    for (auto addon: addons) {
        if (addon.second.isModuleLoaded()) {
            try {
                addon.second.reload();
            } catch (const std::exception &e) {
                if (onAddonLoadFail)
                    onAddonLoadFail(addon.first, e.what());
                rAddons.insert(addon.first);
            }
        }
    }

    auto tmp = loadedModules;

    // Remove addons which failed to reload
    for (auto &r: rAddons)
        tmp.erase(r);

    setActiveAddons({});

    // Reload available addon definitions
    readAddons();

    auto mods = std::set<std::string>();

    for (const auto &addon: tmp) {
        auto it = addons.find(addon);
        if (it != addons.end()) {
            // Check for addons which are not available anymore
            mods.insert(addon);
        }
    }

    setActiveAddons(mods);
}

const std::map<std::string, Addon> &AddonManager::getAvailableAddons() const {
    return addons;
}

std::map<std::string, Addon> &AddonManager::getAvailableAddons() {
    return addons;
}

void AddonManager::setActiveAddons(const std::set<std::string> &inputModules) {
    for (auto &module: loadedModules) {
        if (inputModules.find(module) == inputModules.end()) {
            try {
                addons.at(module).unload();
            } catch (const std::exception &e) {
                if (onAddonUnloadFail) {
                    onAddonUnloadFail(module, e.what());
                }
            }
        }
    }

    std::set<std::string> rMods;
    for (auto &module: inputModules) {
        if (loadedModules.find(module) == loadedModules.end()) {
            try {
                addons.at(module).load();
            } catch (const std::exception &e) {
                if (onAddonLoadFail) {
                    onAddonLoadFail(module, e.what());
                }
                rMods.insert(module);
            }
        }
    }

    loadedModules = inputModules;

    //Remove modules which failed to load
    for (auto &mod: rMods)
        loadedModules.erase(mod);
}

std::set<std::string> AddonManager::getActiveAddons() {
    return loadedModules;
}

void AddonManager::readAddons() {
    auto ad = readAvailableAddons(addonDir);
    for (const auto &addon: ad) {
        bool lod = addons[addon.first].isModuleLoaded();
        addons[addon.first] = Addon(addon.first, addon.second.displayName, addon.second.description);
        if (lod)
            addons[addon.first].setModuleLoaded();
    }
    std::set<std::string> del;
    for (auto &p: addons) {
        if (ad.find(p.first) == ad.end()) {
            del.insert(p.first);
        }
    }
    for (auto &a: del)
        addons.erase(a);
}
