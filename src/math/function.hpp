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

#ifndef QCALC_FUNCTION_HPP
#define QCALC_FUNCTION_HPP

#include <string>
#include <vector>

#include "extern/json.hpp"

struct Function {
    std::string expression;
    std::vector<std::string> argumentNames;

    Function() : expression(), argumentNames() {};

    Function(std::string expression, std::vector<std::string> arguments)
            : expression(std::move(expression)), argumentNames(std::move(arguments)) {}

    bool operator==(const Function &other) const {
        return expression == other.expression && argumentNames == other.argumentNames;
    }
};

#endif //QCALC_FUNCTION_HPP
