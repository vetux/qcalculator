"""
{
  "displayName": "Nilakantha Series",
  "description": "Provides nilakantha series pi calculation script functions"
}
"""

# QCalc - Extensible programming calculator
# Copyright (C) 2021  Julian Zampiccoli
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

# Run the nilakantha series for the specified number of iterations and return the value.
def nilakantha(iterations):
    ret = 3
    for i in range(iterations):
        v = 4 / ((2 + 2 * i) * (3 + 2 * i) * (4 + 2 * i))
        if i % 2 == 0:
            ret += v
        else:
            ret -= v
    return ret

def callback(*args):
    return nilakantha(int(args[0]))

def load():
    sym = exprtk.get_global_symtable()
    sym.set_script("nilakantha", exprtk.ScriptFunction(callback, True))
    exprtk.set_global_symtable(sym)


def unload():
    sym = exprtk.get_global_symtable()
    sym.remove("nilakantha")
    exprtk.set_global_symtable(sym)
