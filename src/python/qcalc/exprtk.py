import qc_native_exprtk
import qcalc.sym


def evaluate(expression, symtable=None):
    if symtable is None:
        symtable = qcalc.sym.SymbolTable()
    ret = evaluate_with_side_effects(expression, symtable)
    return ret[0]


# Returns a tuple with ret[0] being the result of the expression and ret[1] being the updated symbol table
# in case the expression modifies variables.
def evaluate_with_side_effects(expression, symtable):
    return qc_native_exprtk.evaluate(expression, symtable)
