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

#ifndef QT_CALC_SERIALIZER_HPP
#define QT_CALC_SERIALIZER_HPP

#include <string>
#include <set>

#include "math/symboltable.hpp"

#include "settings.hpp"

namespace Serializer {
    std::string serializeTable(const SymbolTable &table);

    SymbolTable deserializeTable(const std::string &str, int precision);

    std::string serializeSettings(const Settings &settings);

    Settings deserializeSettings(const std::string &str);

    std::string serializeSet(const std::set<std::string> &set);

    std::set<std::string> deserializeSet(const std::string &str);

    int serializeRoundingMode(mpfr_rnd_t mode);

    mpfr_rnd_t deserializeRoundingMode(int mode);
}

#endif //QT_CALC_SERIALIZER_HPP
