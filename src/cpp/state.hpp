#ifndef QCALC_STATE_HPP
#define QCALC_STATE_HPP

#include <string>
#include <QSize>

#include "valuetype.hpp"
#include "symboltable.hpp"

struct State {
    ValueType value = 0;
    std::vector<std::pair<std::string, ValueType>> history = {};

    std::string input;
    SymbolTable symbolTable = {};

    bool showKeypad = false;
    bool showBitView = false;
    bool showDock = false;
    int historyLimit = 0;
    int dockPosition = 0;
    int dockSelectedTab = 0;
    QSize windowSize = {};

    int currentVariable = -1;
    int currentConstant = -1;
    int currentFunction = -1;
    int currentScript = -1;
};

#endif //QCALC_STATE_HPP
