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

#ifndef QCALC_UIDGENERATOR_HPP
#define QCALC_UIDGENERATOR_HPP

#include <vector>

#include "util/uid.hpp"

/**
 * Generates unique identifiers which are guaranteed to be unique for each generator instance.
 * Users may optionally return identifiers to make them available in future generation requests.
 */
class UIDGenerator {
public:
    UID get() {
        if (availableIdentifiers.empty()) {
            if (counter == std::numeric_limits<UID>::max() - 1)
                throw std::runtime_error("Maximum amount of identifiers reached");
            return ++counter;
        } else {
            UID ret = *availableIdentifiers.begin();
            availableIdentifiers.erase(availableIdentifiers.begin());
            return ret;
        }
    }

    void put(UID id) {
        if (id == UID_NULL) {
            throw std::runtime_error("UID_NULL passed");
        }

        if (id >= counter) {
            throw std::runtime_error("Invalid id passed");
        }

        if (std::find(availableIdentifiers.begin(), availableIdentifiers.end(), id) != availableIdentifiers.end()) {
            throw std::runtime_error("UID already stored");
        }

        availableIdentifiers.emplace_back(id);
    }

private:
    UID counter = UID_NULL;
    std::vector<UID> availableIdentifiers;
};

#endif //QCALC_UIDGENERATOR_HPP
