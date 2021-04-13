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

#ifndef QCALC_ADDONHELPER_HPP
#define QCALC_ADDONHELPER_HPP

#include <string>
#include <map>

#include "addonmetadata.hpp"

namespace AddonHelper {
    /**
     * Find all python modules (non recursive) in the specified addon directory,
     * look for a corresponding modulename.json file, if the json file exists extract
     * the metadata otherwise set default metadata and return a map containing
     * the metadata objects and the module names as keys.
     *
     * @param addonDirectory The directory to search for addon module and metadata files.
     * @return The map of available addon metadata objects identified by the module name as key.
     */
    std::map<std::string, AddonMetadata> getAvailableAddons(const std::string &addonDirectory);

    /**
     * Import the module and call the load function.
     *
     * @param moduleName
     */
    void load(const std::string &moduleName);

    /**
     * Import the module and call the unload function.
     *
     * @param moduleName
     */
    void unload(const std::string &moduleName);
}

#endif //QCALC_ADDONHELPER_HPP
