import qcalc.sym
import qc_native_presenter as qcpr


def set_presenter_symboltable(symtable):
    if isinstance(symtable, qcalc.sym.SymbolTable):
        return qcpr.setSymbolTable(symtable.table)
    else:
        raise RuntimeError("Symbol table type has to be qcalc.sym.SymbolTable")


def get_presenter_symboltable():
    return qcalc.sym.SymbolTable(qcpr.getSymbolTable())
