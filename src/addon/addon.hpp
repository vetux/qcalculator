/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef QCALC_ADDON_HPP
#define QCALC_ADDON_HPP

#include <string>

class Addon {
public:
    Addon() = default;

    Addon(std::string moduleName, std::string displayName, std::string description);

    void load() {
        callFunctionNoArgs("load");
        loaded = true;
    }

    void unload() {
        callFunctionNoArgs("unload");
        loaded = false;
    }

    void reload();

    void callFunctionNoArgs(const std::string &name);

    bool isLoaded() const { return loaded; }

    bool isModuleLoaded() const { return moduleLoaded; }

    void setModuleLoaded() { moduleLoaded = true; }

    const std::string &getModuleName() const { return moduleName; }

    const std::string &getDisplayName() const { return displayName; }

    const std::string &getDescription() const { return description; }

private:
    bool loaded = false;
    bool moduleLoaded = false;
    std::string moduleName;
    std::string displayName;
    std::string description;
};

#endif //QCALC_ADDON_HPP
