import qcalc.exprtk as exprtk
import qcalc.sym as symbols


def load():
    print("Loading exprtk sample addon")

    val = exprtk.evaluate("1 + 1")
    print("Result: " + str(val))

    sym = symbols.SymbolTable()
    sym.set_variable("x", 42)
    val = exprtk.evaluate("1 + x", sym)
    print("Result: " + str(val))

    val = exprtk.evaluate_with_side_effects("1 + x; x := 631; x - 630", sym)
    print("Result: " + str(val[0]))
    print("Variable: " + str(val[1].get_variable("x")))


def unload():
    print("Unloading exprtk sample addon")


print("Importing exprtk sample addon")
