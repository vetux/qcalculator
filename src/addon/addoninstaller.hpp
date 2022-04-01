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

#ifndef QCALC_ADDONINSTALLER_HPP
#define QCALC_ADDONINSTALLER_HPP

#include <string>

namespace AddonInstaller {
    /**
     * This routine attempts to install an addon by reading some source file and dropping the
     * appropriate files in the addons directory.
     *
     * @param sourceFile
     * @param addonsDirectory
     */
    void installAddon(std::string &sourceFile, const std::string &addonsDirectory);
}

#endif //QCALC_ADDONINSTALLER_HPP
