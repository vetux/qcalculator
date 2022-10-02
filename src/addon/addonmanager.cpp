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
#include <fstream>

#include "io/fileoperations.hpp"

#include "pycx/interpreter.hpp"

#include "extern/json.hpp"

#include "io/archive.hpp"

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

static std::map<std::string, AddonMetadata> readAvailableAddons(const std::string &addonsDirectory) {
    std::map<std::string, AddonMetadata> ret;

    for (auto &addonDir: std::filesystem::directory_iterator(addonsDirectory)) {
        std::string addonDirPath = addonDir.path().string();
        std::vector<std::string> addonFiles = FileOperations::findFilesInDirectory(addonDirPath, ".py");
        for (auto &filePath: addonFiles) {
            auto fileContents = FileOperations::fileReadAllText(filePath);
            std::string n = R"(""")";

            std::string json;
            auto start = fileContents.find_first_of(n);
            if (start != std::string::npos) {
                auto end = fileContents.find(n, start + n.length());
                if (end != std::string::npos) {
                    json = fileContents.substr(start + n.length(), end - n.length());
                }
            }

            auto file = std::filesystem::path(filePath);
            auto fileName = file.filename().string();
            auto fileExt = file.extension().string();

            auto moduleName = fileName.substr(0, fileName.length() - fileExt.length());

            AddonMetadata metadata;
            if (!json.empty()) {
                //Has metadata file
                try {
                    metadata = deserializeMetadata(json);
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
    }

    return ret;
}

void AddonManager::createInstallableBundle(std::vector<std::pair<std::string, std::set<std::string>>> addonPaths) {
    Archive archive;
    for (auto &addon: addonPaths) {

    }
    archive.addEntry()
}

AddonManager::AddonManager(const std::string &addonDirectory,
                           Listener onAddonLoadFail,
                           Listener onAddonUnloadFail)
        : addonDir(addonDirectory),
          onAddonLoadFail(std::move(onAddonLoadFail)),
          onAddonUnloadFail(std::move(onAddonUnloadFail)) {
    reloadModules();
}

AddonManager::~AddonManager() {
    setActiveAddons({});
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

    unloadAddonLibraryPaths();
    loadAddonLibraryPaths();
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

static void createPath(const std::string &path) {
    std::filesystem::path p(path);
    std::string dir = path.substr(0,
                                  path.length() - p.filename().string().length());
    std::filesystem::create_directories(dir);
}

static std::string concatPath(const std::string &target,
                              const std::string &path) {
    char separator = '/';
    if (!path.empty() && path.front() != separator) {
        return target + separator + path;
    } else {
        return target + path;
    }
}

static void writeToFile(const std::string &path,
                        const std::vector<char> &data) {
    std::ofstream ofs(path);
    ofs.write(data.data(), data.size());
}

size_t AddonManager::installAddonBundle(std::istream &sourceFile,
                                        const std::function<bool(const std::string &, const std::string &,
                                                                 std::vector<std::string> &)> &installDialog) {
    const char *const metadataFilePath = "addon_install.json";

    Archive arch(sourceFile);

    if (arch.entries().find(metadataFilePath) == arch.entries().end()) {
        throw std::runtime_error("metadata.json must be present in the addon package");
    }

    auto metadata = arch.entries().at(metadataFilePath);

    std::vector<AddonBundleEntry> bundleEntries;

    auto j = nlohmann::json::parse(metadata);
    if (j.find("addons") != j.end()) {
        for (auto &addon: j["addons"]) {
            AddonBundleEntry bundleEntry;
            bundleEntry.modulePath = addon["module"];
            bundleEntry.module = std::filesystem::path(bundleEntry.modulePath).stem();
            bundleEntry.version = addon.value("version", 0);

            if (addon.find("packages") != addon.end()) {
                for (auto &jLib: addon["packages"]) {
                    auto libraryDirectory = jLib.get<std::string>();
                    bundleEntry.packages.emplace_back(libraryDirectory);
                }
            }

            bundleEntries.emplace_back(bundleEntry);
        }

        std::vector<std::string> addonModules;

        for (auto &entry: bundleEntries) {
            addonModules.emplace_back(entry.module + " v" + std::to_string(entry.version));
        }

        if (!installDialog("Install addons", "Select the addons you want to install", addonModules)) {
            return 0;
        }

        auto copy = std::vector<AddonBundleEntry>();
        for (auto &entry: bundleEntries) {
            if (std::find(addonModules.begin(), addonModules.end(),
                          entry.module + " v" + std::to_string(entry.version)) != addonModules.end()) {
                copy.emplace_back(entry);
            }
        }
        bundleEntries = copy;

        for (auto &addon: bundleEntries) {
            std::filesystem::path addonFile(addon.modulePath);

            //Install addon

            if (addonFile.extension() != ".py") {
                throw std::runtime_error("Invalid extension for addon file " + addonFile.string());
            }

            auto outputDir = concatPath(addonDir, addonFile.stem().string());
            auto outputPath = concatPath(outputDir, addonFile.filename().string());

            std::filesystem::remove_all(outputPath);

            createPath(outputPath);
            writeToFile(outputPath, arch.entries().at(addon.modulePath));

            // Install addon packages
            for (auto &package: addon.packages) {
                auto libraryDirectory = package;
                if (!libraryDirectory.empty() && libraryDirectory.back() == '/')
                    libraryDirectory.pop_back();

                auto libraryPackage = std::filesystem::path(libraryDirectory).filename();

                std::set<std::string> libFiles;
                for (auto &entry: arch.entries()) {
                    if (entry.first.size() > libraryDirectory.size() && entry.first.find(libraryDirectory) == 0) {
                        libFiles.insert(entry.first);
                    }
                }

                if (!libFiles.empty()) {
                    for (auto &libFilePath: libFiles) {
                        auto path = concatPath(outputDir, libFilePath.substr(libraryDirectory.size()));
                        createPath(path);
                        writeToFile(path, arch.entries().at(libFilePath));
                    }
                } else {
                    throw std::runtime_error("No packages files found for defined package " + libraryDirectory);
                }
            }
        }
    }

    return bundleEntries.size();
}

void AddonManager::uninstallAddon(const std::string &moduleName) {
    if (loadedModules.find(moduleName) != loadedModules.end()) {
        auto v = loadedModules;
        v.erase(moduleName);
        setActiveAddons(v);
    }

    auto addonFile = addonDir + "/" + moduleName;

    std::filesystem::remove_all(addonFile);

    addons.erase(moduleName);
    loadedModules.erase(moduleName);
}

void AddonManager::loadAddonLibraryPaths() {
    for (auto &entry: std::filesystem::directory_iterator(addonDir)) {
        if (entry.is_directory()) {
            auto path = entry.path().string();
            addonLibraryPaths.insert(path);
            Interpreter::addModuleDir(path);
        }
    }
}

void AddonManager::unloadAddonLibraryPaths() {
    for (auto &path: addonLibraryPaths) {
        Interpreter::removeModuleDir(path);
    }
    addonLibraryPaths.clear();
}
