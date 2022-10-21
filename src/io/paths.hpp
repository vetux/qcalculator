/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef QCALC_PATHS_HPP
#define QCALC_PATHS_HPP

#include <string>

namespace Paths {
    static const std::string ADDONS_FILE = "/addons.json";
    static const std::string SETTINGS_FILE = "/settings.json";
    static const std::string SYMBOL_TABLE_HISTORY_FILE = "/sym_path_history.txt";
    static const std::string HISTORY_FILE = "/history.txt";
    static const std::string CALCULATOR_ICON_FILE = "/icons/calculator.ico";
    static const std::string SYMBOLS_ICON_FILE = "/icons/symbols.ico";
    static const std::string TERMINAL_ICON_FILE = "/icons/terminal.ico";

    std::string getApplicationDirectory();

    std::string getAppDataDirectory();

    std::string getAppConfigDirectory();

    std::string getAddonDirectory();

    std::string getLibDirectory();

    inline std::string getAddonsFile() {
        return getAppConfigDirectory() + ADDONS_FILE;
    }

    inline std::string getSettingsFile() {
        return getAppConfigDirectory() + SETTINGS_FILE;
    }

    inline std::string getSymbolTableHistoryFile() {
        return getAppConfigDirectory() + SYMBOL_TABLE_HISTORY_FILE;
    }

    inline std::string getHistoryFile() {
        return getAppConfigDirectory() + HISTORY_FILE;
    }

    inline std::string getCalculatorIconFile() {
        return getApplicationDirectory() + CALCULATOR_ICON_FILE;
    }

    inline std::string getSymbolsIconFile() {
        return getApplicationDirectory() + SYMBOLS_ICON_FILE;
    }

    inline std::string getTerminalIconFile() {
        return getApplicationDirectory() + TERMINAL_ICON_FILE;
    }
}

#endif //QCALC_PATHS_HPP
