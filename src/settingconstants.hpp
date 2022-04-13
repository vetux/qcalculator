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

const char *const SETTING_KEY_PRECISION = "_qcalc_precision_internal";
const int SETTING_DEFAULT_PRECISION = 4000;

const char *const SETTING_KEY_ROUNDING = "_qcalc_rounding_internal";
const int SETTING_DEFAULT_ROUNDING = 0;

const char *const SETTING_KEY_PRECISION_F = "_qcalc_precision_format";
const int SETTING_DEFAULT_PRECISION_F = 100;

const char *const SETTING_KEY_ROUNDING_F = "_qcalc_rounding_format";
const int SETTING_DEFAULT_ROUNDING_F = 0;

const char *const SETTING_KEY_SAVE_SYM_HISTORY = "_qcalc_save_sym_hist";
const int SETTING_DEFAULT_SAVE_SYM_HISTORY = true;

#endif //QCALC_SETTINGCONSTANTS_HPP
