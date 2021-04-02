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


def remove(name):
    return qcst.remove(name)


def get_variable_names():
    return qcst.getVariableNames()


def get_variable(name):
    return qcst.getVariable(name)


def set_variable(name, value):
    return qcst.setVariable(name, value)


def get_constant_names():
    return qcst.getConstantNames()


def get_constant(name):
    return qcst.getConstant(name)


def set_constant(name, value):
    return qcst.setConstant(name, value)


def get_function_names():
    return qcst.getFunctionNames()


def get_function(name):
    ret = Function()
    ret.expression = qcst.getFunctionExpression(name)
    ret.argument_names = qcst.getFunctionArgumentNames(name)
    return ret


def set_function(name, func):
    return qcst.setFunction(name, func.expression, func.argument_names)


def get_script_names():
    return qcst.getScriptFunctionNames()


def get_script(name):
    ret = ScriptFunction
    ret.callback = qcst.getScriptFunctionCallback(name)
    ret.enable_arguments = qcst.getScriptFunctionEnableArguments(name)
    return ret


def set_script(name, script):
    return qcst.setScriptFunction(name, script.callback, script.enable_arguments)
