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

import _exprtk

class Function:
    def __init__(self, expression="", arguments=[]):
        if arguments is None:
            arguments = []
        self.expression = expression
        self.arguments = arguments


class ScriptFunction:
    def __init__(self, callback=None, arguments=[]):
        self.callback = callback
        self.arguments = arguments


class SymbolTable:
    def __init__(self, variables=None, constants=None, functions=None, scripts=None):
        if variables is None:
            variables = {}
        if constants is None:
            constants = {}
        if functions is None:
            functions = {}
        if scripts is None:
            scripts = {}
        self.variables = variables
        self.constants = constants
        self.functions = functions
        self.scripts = scripts

    def remove(self, name):
        self.variables.pop(name, None)
        self.constants.pop(name, None)
        self.functions.pop(name, None)
        self.scripts.pop(name, None)

    def get_variable_names(self):
        return self.variables.keys()

    def get_variable(self, name):
        return self.variables[name]

    def set_variable(self, name, value):
        self.variables[name] = value

    def get_constant_names(self):
        return self.constants.keys()

    def get_constant(self, name):
        return self.constants[name]

    def set_constant(self, name, value):
        self.constants[name] = value

    def get_function_names(self):
        return self.functions.keys()

    def get_function(self, name):
        return self.functions[name]

    def set_function(self, name, func):
        self.functions[name] = func

    def get_script_names(self):
        return self.scripts.keys()

    def get_script(self, name):
        return self.scripts[name]

    def set_script(self, name, script):
        self.scripts[name] = script

    def set_script_noargs(self, name, callback):
        self.scripts[name] = ScriptFunction(callback)


def evaluate(expression, symtable=None):
    if symtable is None:
        symtable = SymbolTable()
    ret = evaluate_with_side_effects(expression, symtable)
    return ret[0]


# Returns a tuple with ret[0] being the result of the expression and ret[1] being the updated symbol table
# in case the expression modifies variables.
def evaluate_with_side_effects(expression, symtable):
    return _exprtk.evaluate(expression, symtable)


def get_global_symtable():
    return _exprtk.get_global_symtable()


def set_global_symtable(sym):
    return _exprtk.set_global_symtable(sym)
