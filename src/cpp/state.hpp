#ifndef QCALC_STATE_HPP
#define QCALC_STATE_HPP

#include <string>
#include <QSize>

#include "valuetype.hpp"
#include "symboltable.hpp"
#include "observable.hpp"

/**
 * The minimum data to represent the application state.
 * The presenter uses other "model" classes to transform the state
 * into view data such as strings.
 *
 * For example a search function for the symbol table would be the search string in the state.
 * The presenter then uses some other model class to extract the filtered symbols, and gives
 * the view the filtered data.
 */
struct State {
    ValueType value = 0;

    std::vector<std::pair<std::string, ValueType>> history = {};

    std::string input;

    SymbolTable symbolTable = {};

    bool showKeypad = false;
    bool showBitView = false;
    bool showDock = false;

    int historyLimit = 0;

    Qt::DockWidgetArea dockPosition = Qt::DockWidgetArea::BottomDockWidgetArea;
    int dockSelectedTab = 0;

    QSize windowSize = {};

    int currentVariable = -1;
    int currentConstant = -1;
    int currentFunction = -1;
    int currentScript = -1;
};

#endif //QCALC_STATE_HPP
