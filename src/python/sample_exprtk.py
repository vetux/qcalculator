import qcalc.exprtk


class CustomCallbacks:
    def __init__(self):
        pass

    # Our script function callback which is invoked by the expression parser
    # when it encounters the defined symbol name in a expression.
    def evaluate(self, *args):
        return 422

    # Our script function callback which takes at least one argument
    def evaluate_args(self, *args):
        return args[0]


callbacks = CustomCallbacks


def load():
    print("Loading exprtk sample addon")

    global callbacks

    callbacks = CustomCallbacks()

    sym = qcalc.exprtk.SymbolTable()

    sym.set_variable("pyVar", 0)
    sym.set_constant("pyConst", 3.141)

    # A native function with no arguments
    sym.set_function("pyFunc", qcalc.exprtk.Function("42 + sin(32)"))

    # A native function with 1 argument called "arg1"
    sym.set_function("pyFuncArgs", qcalc.exprtk.Function("133 + cos(arg1)", {"arg1"}))

    # A script function with no arguments
    sym.set_script_noargs("pyScript", callbacks.evaluate)

    # A script function which accepts at least 1 argument
    sym.set_script("pyScriptArgs", qcalc.exprtk.ScriptFunction(callbacks.evaluate_args, True))

    result = qcalc.exprtk.evaluate_with_side_effects(
        "pyVar := pyConst / pyFunc + pyFuncArgs(42) + pyScript + pyScriptArgs(42, 13)", sym)

    print("Variable: " + str(result[1].get_variable("pyVar")))


def unload():
    print("Unloading exprtk sample addon")


print("Importing exprtk sample addon")
