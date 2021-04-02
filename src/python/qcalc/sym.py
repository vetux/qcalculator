import qc_native_symtable as qcst


class Function:
    def __init__(self, expression=None, argument_names=None):
        if argument_names is None:
            argument_names = []
        self.expression = expression
        self.argument_names = argument_names


class ScriptFunction:
    def __init__(self, callback=None, enable_arguments=False):
        self.callback = callback
        self.enable_arguments = enable_arguments


class SymbolTable:
    def __init__(self, table = None):
        if table is None:
            self.table = qcst.NativeSymbolTable()
        else:
            self.table = table

    def remove(self, name):
        return self.table.removeSymbol(name)

    def get_variable_names(self):
        return self.table.getVariableNames()

    def get_variable(self, name):
        return self.table.getVariable(name)

    def set_variable(self, name, value):
        return self.table.setVariable(name, value)

    def get_constant_names(self):
        return self.table.getConstantNames()

    def get_constant(self, name):
        return self.table.getConstant(name)

    def set_constant(self, name, value):
        return self.table.setConstant(name, value)

    def get_function_names(self):
        return self.table.getFunctionNames()

    def get_function(self, name):
        ret = Function()
        ret.expression = self.table.getFunctionExpression(name)
        ret.argument_names = self.table.getFunctionArgumentNames(name)
        return ret

    def set_function(self, name, func):
        return self.table.setFunction(name, func.expression, func.argument_names)

    def get_script_names(self):
        return self.table.getScriptFunctionNames()

    def get_script(self, name):
        ret = ScriptFunction
        ret.callback = self.table.getScriptFunctionCallback(name)
        ret.enable_arguments = self.table.getScriptFunctionEnableArguments(name)
        return ret

    def set_script(self, name, script):
        return self.table.setScriptFunction(name, script.callback, script.enable_arguments)
