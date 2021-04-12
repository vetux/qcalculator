import math
import qcalc.exprtk


def callback(*args):
    return math.factorial(int(args[0]))


def load():
    sym = qcalc.exprtk.get_global_symtable()
    sym.set_script("factorial", qcalc.exprtk.ScriptFunction(callback, True))
    qcalc.exprtk.set_global_symtable(sym)


def unload():
    sym = qcalc.exprtk.get_global_symtable()
    sym.remove("factorial")
    qcalc.exprtk.set_global_symtable(sym)
