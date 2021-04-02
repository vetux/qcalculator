import qcalc.sym as sym

# ! IMPORTANT ! The user may import / export symbol tables, so handle variables, constants and functions with care
# (eg. only in a registered script function.)
# Also the user is able to modify the symbol table through the gui.
# Scripts are not serialized.
# The script will be unloaded before import and loaded again after import.


# The script function callback which is invoked by the expression parser
# when it encounters our symbol name in a expression.
def evaluate(*args):
    sym.set_variable("runtimeVar", 41)
    return 422


# A script function callback which takes at least one argument
def evaluate_args(*args):
    return args[0]


def load():
    print("Loading sample sym module")

    sym.set_variable("pyVar", 42)
    sym.set_constant("pyConst", 3.141)

    # A native function with no arguments
    sym.set_function("pyFunc", sym.Function("42 + sin(32)"))

    # A native function with 1 argument called "arg1"
    sym.set_function("pyFuncArgs", sym.Function("133 + cos(arg1)", {"arg1"}))

    # A script function with no arguments
    sym.set_script("pyScript", sym.ScriptFunction(evaluate))

    # A script function which accepts at least 1 argument
    sym.set_script("pyScriptArgs", sym.ScriptFunction(evaluate_args, True))


def unload():
    print("Unloading sample sym module")
    sym.remove("pyVar")
    sym.remove("pyConst")
    sym.remove("pyFunc")
    sym.remove("pyFuncArgs")
    sym.remove("pyScript")
    sym.remove("pyScriptArgs")


print("Sample SYM Addon has been imported")