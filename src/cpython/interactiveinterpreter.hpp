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

#ifndef QCALC_INTERACTIVEINTERPRETER_HPP
#define QCALC_INTERACTIVEINTERPRETER_HPP

#include <string>

namespace InteractiveInterpreter {
    void initialize();

    int run();

    enum ParseStyle {
        SINGLE_INPUT,
        FILE_INPUT,
        EVAL_INPUT,
        FUNC_TYPE_INPUT
    };

    /**
     * Run the python code in the expression argument in the context module and return the string value
     * of the result.
     *
     * @param expression
     * @return
     */
    std::string runString(const std::string &expression, ParseStyle style = SINGLE_INPUT, const std::string &context = "__main__");
}

#endif //QCALC_INTERACTIVEINTERPRETER_HPP
