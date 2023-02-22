/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef QCALCULATOR_CALCULATORWINDOWACTIONS_HPP
#define QCALCULATOR_CALCULATORWINDOWACTIONS_HPP

#include <QMenu>
#include <QAction>

struct CalculatorWindowActions {
    QAction *actionSettings{};
    QAction *actionExit{};

    QAction *actionClearHistory{};

    QAction *actionOpenTerminal{};

    QAction *actionEditSymbols{};
    QAction *actionOpenSymbols{};
    QAction *actionSaveSymbols{};
    QAction *actionSaveAsSymbols{};
    QAction *actionClearSymbols{};

    QAction *actionCompressDirectory{};
    QAction *actionExtractArchive{};
    QAction *actionCreateAddonBundle{};

    QAction *actionAbout{};
    QAction *actionAboutQt{};
    QAction *actionAboutPython{};

    QMenu *menuFile{};
    QMenu *menuSymbols{};
    QMenu *menuTools{};
    QMenu *menuHelp{};

    QMenu *menuOpenRecent{};
};

#endif //QCALCULATOR_CALCULATORWINDOWACTIONS_HPP
