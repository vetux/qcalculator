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

const Setting SETTING_PRECISION = {"_qcalc_precision_internal", 100};
const Setting SETTING_EXPONENT_MAX = {"_qcalc_exponent_max", 999999};
const Setting SETTING_EXPONENT_MIN = {"_qcalc_exponent_min", -999999};
const Setting SETTING_ROUNDING = {"_qcalc_rounding_internal", MPD_ROUND_HALF_EVEN};
const Setting SETTING_SAVE_SYM_HISTORY = {"_qcalc_save_sym_hist", true};
const Setting SETTING_WARN_INEXACT = {"_qcalc_warn_inexact", true};
const Setting SETTING_PYTHON_MODPATHS = {"_qcalc_python_modpaths", std::set<std::string>()};

#endif //QCALC_SETTINGCONSTANTS_HPP
