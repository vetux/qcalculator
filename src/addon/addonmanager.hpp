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

#ifndef QCALC_ADDONMANAGER_HPP
#define QCALC_ADDONMANAGER_HPP

#include <set>
#include <string>
#include <functional>
#include <atomic>
#include <map>

#include "addon.hpp"
#include "addon/library.hpp"

class AddonManager {
public:
    typedef std::function<void(const std::string &, const std::string &)> Listener;

    AddonManager(const std::string &addonDirectory,
                 const std::string &libDirectory,
                 Listener onAddonLoadFail,
                 Listener onAddonUnloadFail);

    ~AddonManager();

    void reloadModules();

    const std::map<std::string, Addon> &getAvailableAddons() const;

    std::map<std::string, Addon> &getAvailableAddons();

    /**
     * @param inputModules The set of module names which should currently be loaded.
     */
    void setActiveAddons(const std::set<std::string> &inputModules);

    /**
     * @return The set of currently loaded module names.
     */
    std::set<std::string> getActiveAddons();

    /**
     * @return The set of available libraries.
     */
    std::map<std::string, Library> getLibraries();

    void installAddon(std::istream &sourceFile,
                      std::function<bool(const std::string &, const std::string &)> questionDialog);

    void uninstallAddon(const std::string &moduleName);

    void uninstallLibrary(const std::string &libraryPackage);

private:
    void readAddons();

    std::string addonDir;
    std::string libDir;

    std::map<std::string, Addon> addons;
    std::set<std::string> loadedModules;

    Listener onAddonLoadFail;
    Listener onAddonUnloadFail;

    std::map<std::string, Library> libraries;
};

#endif //QCALC_ADDONMANAGER_HPP
