import qcalc.sym
import qc_native_presenter as qcpr


def set_presenter_symboltable(symtable):
    return qcpr.setSymbolTable(symtable)


def get_presenter_symboltable():
    return qcpr.getSymbolTable()
