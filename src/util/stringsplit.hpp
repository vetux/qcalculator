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

#ifndef QCALCULATOR_STRINGSPLIT_HPP
#define QCALCULATOR_STRINGSPLIT_HPP

#include <vector>
#include <string>

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> ret;
    for (size_t it = 0; it != std::string::npos; it = str.find(delimiter, it + 1)) {
        if (it != 0)
            it += 1;
        auto next = str.find(delimiter, it);
        std::string line;
        if (next == std::string::npos) {
            line = str.substr(it);
        } else {
            line = str.substr(it, next - it);
        }
        ret.emplace_back(line);
    }
    return ret;
}

#endif //QCALCULATOR_STRINGSPLIT_HPP
