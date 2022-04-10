"""
{
  "displayName": "Sample Addon - EXPRTK",
  "description": "A sample addon demonstrating the use of the exprtk api."
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

import exprtk
import mpreal

class CustomCallbacks:
    def __init__(self):
        pass

    # Our script function callback which is invoked by the expression parser
    # when it encounters the defined symbol name in a expression.
    # The arguments are a tuple of mpreal instances.
    # Allowed return values are: float, long and mpreal
    def evaluate(self, *args):
        return 422

    # Our script function callback which takes at least one argument
    def evaluate_args(self, *args):
        return args[0]


callbacks = CustomCallbacks


def load():
    print("Loading exprtk sample addon")

    # When setting the default precision or rounding through a script
    # this also affects the native precision ( But not the output formatting precision.) and
    # user configured rounding mode,
    # and should therefore be reset to the original value before returning control to the native side.
    original_precision = mpreal.mpreal.get_default_precision()
    original_rounding = mpreal.mpreal.get_default_rounding()

    mpreal.mpreal.set_default_precision(9)
    mpreal.mpreal.set_default_rounding(mpreal.RoundingMode.ROUND_AWAY_FROM_ZERO)

    x = mpreal.mpreal(1)
    y = mpreal.mpreal(3)

    z = x / y
    z.set_precision(3)

    print("MpReal Value: " + str(z))

    global callbacks

    callbacks = CustomCallbacks()

    sym = exprtk.SymbolTable()

    sym.set_variable("pyVar", 0)
    sym.set_constant("pyConst", 3.141)

    # A native function with no arguments
    sym.set_function("pyFunc", exprtk.Function("42 + sin(32)"))

    # A native function with 1 argument called "arg1"
    sym.set_function("pyFuncArgs", exprtk.Function("133 + cos(arg1)", {"arg1"}))

    # A script function with no arguments
    sym.set_script_noargs("pyScript", callbacks.evaluate)

    # A script function which accepts at least 1 argument
    sym.set_script("pyScriptArgs", exprtk.ScriptFunction(callbacks.evaluate_args, True))

    result = exprtk.evaluate_with_side_effects("pyVar := pyConst / pyFunc + pyFuncArgs(42) + pyScript + pyScriptArgs(42, 13)",
                                        sym)

    print("Variable: " + str(result[1].get_variable("pyVar")))

    mpreal.mpreal.set_default_precision(original_precision)
    mpreal.mpreal.set_default_rounding(original_rounding)


def unload():
    print("Unloading exprtk sample addon")


print("Importing exprtk sample addon")
