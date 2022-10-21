/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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

static std::string concatPath(const std::string &target,
                              const std::string &path) {
    char separator = '/';
    if (!path.empty() && path.front() != separator) {
        return target + separator + path;
    } else {
        return target + path;
    }
}

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

        auto filePath = concatPath(addonDirPath, addonDir.path().filename().string() + ".py");

        auto fileContents = FileOperations::fileReadAll(filePath);
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

    return ret;
}

Archive AddonManager::createInstallableBundle(std::vector<InstallBundleEntry> entries) {
    Archive archive;
    auto bundle = nlohmann::json();
    for (auto &entry: entries) {
        auto j = nlohmann::json();

        auto packagePath = std::filesystem::path(entry.packagePath);
        auto basePath = packagePath.parent_path();

        if (std::filesystem::is_directory(packagePath)) {
            auto packageName = packagePath.filename().string() + "/";

            j["package"] = packageName;

            for (auto &packageFile: FileOperations::findFilesInDirectory(packagePath, "", true)) {
                auto packageFilePath = packageFile.substr(basePath.string().size() + 1);
                auto packageDataStr = FileOperations::fileReadAll(packageFile);

                std::vector<char> packageData(packageDataStr.size());
                for (auto i = 0; i < packageData.size(); i++) {
                    packageData.at(i) = packageDataStr.at(i);
                }

                archive.addEntry(packageFilePath, packageData);
            }
        } else {
            auto packageFilePath = packagePath.string().substr(basePath.string().size() + 1);

            j["package"] = packageFilePath;

            auto packageDataStr = FileOperations::fileReadAll(packagePath.string());

            std::vector<char> packageData(packageDataStr.size());
            for (auto i = 0; i < packageData.size(); i++) {
                packageData.at(i) = packageDataStr.at(i);
            }

            archive.addEntry(packageFilePath, packageData);
        }

        j["version"] = entry.version;

        bundle["addons"].emplace_back(j);
    }

    bundle["bundleVersion"] = 1;

    std::string addonBundleStr = bundle.dump();
    std::vector<char> addonBundle(addonBundleStr.size());
    for (auto i = 0; i < addonBundle.size(); i++) {
        addonBundle.at(i) = addonBundleStr.at(i);
    }

    archive.addEntry("addon_bundle.json", addonBundle);

    return archive;
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
    auto active = activeAddons;

    // Unload all addons
    setActiveAddons({});

    // Reload available addon definitions
    readAddons();

    // Check for addon reload failure of active addons
    std::set<std::string> workingAddons;
    for (auto &mod: active) {
        if (addons.find(mod) == addons.end()) {
            // Addon is not available on disk anymore.
            continue;
        }
        auto addon = addons.at(mod);
        try {
            addon.reload();
        } catch (const std::exception &e) {
            if (onAddonLoadFail)
                onAddonLoadFail(mod, e.what());
            continue;
        }
        workingAddons.insert(mod);
    }

    // Reload working addons
    setActiveAddons(workingAddons);
}

const std::map<std::string, Addon> &AddonManager::getAvailableAddons() const {
    return addons;
}

std::map<std::string, Addon> &AddonManager::getAvailableAddons() {
    return addons;
}

void AddonManager::setActiveAddons(const std::set<std::string> &inputAddons) {
    for (auto &module: activeAddons) {
        if (inputAddons.find(module) == inputAddons.end()) {
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
    for (auto &module: inputAddons) {
        if (activeAddons.find(module) == activeAddons.end()) {
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

    activeAddons = inputAddons;

    //Remove modules which failed to load
    for (auto &mod: rMods)
        activeAddons.erase(mod);
}

std::set<std::string> AddonManager::getActiveAddons() {
    return activeAddons;
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

static void writeToFile(const std::string &path,
                        const std::vector<char> &data) {
    std::ofstream ofs(path);
    ofs.write(data.data(), data.size());
}

size_t AddonManager::installAddonBundle(std::istream &sourceFile,
                                        const std::function<bool(const std::string &, const std::string &,
                                                                 std::vector<std::string> &)> &installDialog) {
    const char *const metadataFilePath = "addon_bundle.json";

    Archive arch(sourceFile);

    if (arch.entries().find(metadataFilePath) == arch.entries().end()) {
        throw std::runtime_error("addon_bundle.json must be present in the addon package");
    }

    auto metadata = arch.entries().at(metadataFilePath);

    std::vector<AddonBundleEntry> bundleEntries;

    auto j = nlohmann::json::parse(metadata);
    if (j.find("addons") != j.end()) {
        for (auto &addon: j["addons"]) {
            AddonBundleEntry bundleEntry;
            bundleEntry.packagePath = addon["package"];
            bundleEntry.version = addon.value("version", 0);
            bundleEntries.emplace_back(bundleEntry);
        }

        std::vector<std::string> addonModules;

        for (auto &entry: bundleEntries) {
            auto str = entry.packagePath.back() == '/'
                       ? entry.packagePath.substr(0, entry.packagePath.size() - 1)
                       : entry.packagePath;
            auto packagePath = std::filesystem::path(str);
            addonModules.emplace_back(packagePath.filename().string() + " v" + std::to_string(entry.version));
        }

        if (!installDialog("Install addons", "Select the addons you want to install", addonModules)) {
            return 0;
        }

        auto copy = std::vector<AddonBundleEntry>();
        for (auto &entry: bundleEntries) {
            auto str = entry.packagePath.back() == '/'
                       ? entry.packagePath.substr(0, entry.packagePath.size() - 1)
                       : entry.packagePath;
            auto packagePath = std::filesystem::path(str);
            if (std::find(addonModules.begin(), addonModules.end(),
                          packagePath.filename().string() + " v" + std::to_string(entry.version)) !=
                addonModules.end()) {
                copy.emplace_back(entry);
            }
        }
        bundleEntries = copy;

        for (auto &addon: bundleEntries) {
            std::filesystem::path addonPackagePath(addon.packagePath);

            if (addon.packagePath.back() == '/') {
                auto packagePath = std::filesystem::path(addon.packagePath.substr(0, addon.packagePath.size() - 1));
                auto outputDir = concatPath(addonDir, packagePath.filename());

                std::filesystem::remove_all(outputDir);
                createPath(outputDir);

                // Write all files in the package directory to disk
                auto packageDir = addon.packagePath;
                if (!packageDir.empty() && packageDir.back() == '/')
                    packageDir.pop_back();

                std::set<std::string> files;
                for (auto &entry: arch.entries()) {
                    if (entry.first.size() > packageDir.size() && entry.first.find(packageDir) == 0) {
                        files.insert(entry.first);
                    }
                }

                if (!files.empty()) {
                    for (auto &filePath: files) {
                        auto path = concatPath(outputDir, filePath.substr(packageDir.size()));
                        createPath(path);
                        writeToFile(path, arch.entries().at(filePath));
                    }
                } else {
                    throw std::runtime_error("No packages files found for defined package " + packageDir);
                }
            } else {
                auto outputFile = concatPath(addonDir, addonPackagePath.filename().string());

                std::filesystem::remove(outputFile);
                createPath(outputFile);

                // Write the package file to disk
                writeToFile(outputFile, arch.getEntry(addon.packagePath));
            }
        }
    }

    return bundleEntries.size();
}

void AddonManager::uninstallAddon(const std::string &moduleName) {
    if (activeAddons.find(moduleName) != activeAddons.end()) {
        auto v = activeAddons;
        v.erase(moduleName);
        setActiveAddons(v);
    }

    auto addonFile = addonDir + "/" + moduleName;

    std::filesystem::remove_all(addonFile);

    addons.erase(moduleName);
    activeAddons.erase(moduleName);
}
