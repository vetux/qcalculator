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

#include "addonmanagerlistener.hpp"

namespace AddonManager {
    /**
     * This function ensures that the addon modules in the passed set are currently loaded,
     * and unloads any modules which are not contained in the set.
     *
     * If an addon fails to load it will still be added to the active addons and unloaded when not contained
     * in a subsequent call to setActiveAddons.
     *
     * @param addons The set of module names which should currently be loaded.
     * @param listener The listener instance to invoke callbacks on in case of errors.
     */
    void setActiveAddons(const std::set<std::string> &addons, AddonManagerListener &listener);

    /**
     * @return The set of currently loaded modules.
     */
    std::set<std::string> getActiveAddons();
}

#endif //QCALC_ADDONMANAGER_HPP
