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

#ifndef QCALC_SETTINGCONSTANTS_HPP
#define QCALC_SETTINGCONSTANTS_HPP

#include "settings/settings.hpp"

const Setting SETTING_PRECISION = {"precision", 100};
const Setting SETTING_EXPONENT_MAX = {"exponent_max", 999999};
const Setting SETTING_EXPONENT_MIN = {"exponent_min", -999999};
const Setting SETTING_ROUNDING = {"rounding", MPD_ROUND_HALF_EVEN};
const Setting SETTING_SAVE_SYMBOLS_HISTORY = {"save_symbols_history", true};
const Setting SETTING_PYTHON_MODULE_PATHS = {"python_module_paths", std::set<std::string>()};
const Setting SETTING_PYTHON_PATH = {"python_path", std::string()};
const Setting SETTING_SAVE_HISTORY =  {"save_history", true};

#endif //QCALC_SETTINGCONSTANTS_HPP
