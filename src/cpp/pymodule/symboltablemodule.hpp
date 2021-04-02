#ifndef QCALC_SYMBOLTABLEMODULE_HPP
#define QCALC_SYMBOLTABLEMODULE_HPP

class MainPresenter;

/**
 * A module which makes the symbol table type available to python.
 */
class SymbolTableModule {
public:
    /**
     * Register a new native python module.
     * Should be called once before initializing python.
     */
    static void initialize(MainPresenter &presenter);
};

#endif //QCALC_SYMBOLTABLEMODULE_HPP
