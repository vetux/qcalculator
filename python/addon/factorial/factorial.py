"""
{
  "displayName": "Factorial",
  "description": "This addon adds the factorial function as a script function."
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


def callback(*args):
    return math.factorial(int(args[0]))


def load():
    sym = exprtk.get_global_symtable()
    sym.set_script("factorial", exprtk.ScriptFunction(callback, ["n"]))
    exprtk.set_global_symtable(sym)


def unload():
    sym = exprtk.get_global_symtable()
    sym.remove("factorial")
    exprtk.set_global_symtable(sym)
