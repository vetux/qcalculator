"""
{
  "displayName": "Nilakantha Series",
  "description": "Provides nilakantha series pi calculation script function"
}
"""

# QCalc - Extensible programming calculator
# Copyright (C) 2023  Julian Zampiccoli
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import math
import exprtk
import decimal

# Run the nilakantha series for the specified number of iterations and return the value.
def nilakantha(iterations):
    ret = decimal.Decimal(3)
    for i in range(iterations):
        v = decimal.Decimal(4) / ((decimal.Decimal(2) + decimal.Decimal(2) * decimal.Decimal(i)) * (decimal.Decimal(3) + decimal.Decimal(2) * decimal.Decimal(i)) * (decimal.Decimal(4) + decimal.Decimal(2) * decimal.Decimal(i)))
        if decimal.Decimal(i) % decimal.Decimal(2) == decimal.Decimal(0):
            ret += v
        else:
            ret -= v
    return str(ret)

def callback(*args):
    return nilakantha(int(args[0]))

def load():
    sym = exprtk.get_global_symtable()
    sym.set_script("nilakantha", exprtk.ScriptFunction(callback, ["iterations"]))
    exprtk.set_global_symtable(sym)


def unload():
    sym = exprtk.get_global_symtable()
    sym.remove("nilakantha")
    exprtk.set_global_symtable(sym)
