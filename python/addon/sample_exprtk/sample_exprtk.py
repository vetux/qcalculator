"""
{
  "displayName": "Sample Addon - EXPRTK",
  "description": "A sample addon demonstrating the use of the exprtk api."
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

import exprtk
import decimal

class CustomCallbacks:
    def __init__(self):
        pass

    # Our script function callback which is invoked by the expression parser
    # when it encounters the defined symbol name in a expression.
    # The arguments are a tuple of mpreal instances.
    # Allowed return values are: float, long and mpreal
    def evaluate(self):
        return 422

    # Our script function callback which takes at least one argument
    def evaluate_args(self, *args):
        return args[0]


callbacks = CustomCallbacks


def load():
    print("Loading exprtk sample addon")

    global callbacks

    callbacks = CustomCallbacks()

    sym = exprtk.SymbolTable()

    sym.set_variable("pyVar", 0)
    sym.set_constant("pyConst", 3.141)

    # A native function with no arguments
    sym.set_function("pyFunc", exprtk.Function("42 + 32"))

    # A native function with 1 argument called "arg1"
    sym.set_function("pyFuncArgs", exprtk.Function("133 + arg1", ["arg1"]))

# A script function with no arguments
    sym.set_script_noargs("pyScript", callbacks.evaluate)

# A script function which accepts at least 1 argument
    # The script function argument names that are displayed to the user are specified here
    sym.set_script("pyScriptArgs", exprtk.ScriptFunction(callbacks.evaluate_args, ["myArgName"]))

    result = exprtk.evaluate_with_side_effects("pyVar := pyConst / pyFunc() + pyFuncArgs(42) + pyScript() + pyScriptArgs(42, 13)",
                                        sym)

    print("Variable: " + str(result[1].get_variable("pyVar")))


def unload():
    print("Unloading exprtk sample addon")


print("Importing exprtk sample addon")
