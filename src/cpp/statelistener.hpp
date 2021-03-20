#ifndef QCALC_STATELISTENER_HPP
#define QCALC_STATELISTENER_HPP

#include "state.hpp"

class StateListener {
public:
    virtual void onStateValueChanged(ValueType value) = 0;

    virtual void onStateHistoryChanged(std::vector<std::pair<std::string, ValueType>> value) = 0;

    virtual void onStateInputChanged(std::string value) = 0;

    virtual void onStateSymbolTableChanged(SymbolTable value) = 0;

    virtual void onStateShowKeyPadChanged(bool value) = 0;

    virtual void onStateShowBitViewChanged(bool value) = 0;

    virtual void onStateShowDockChanged(bool value) = 0;

    virtual void onStateHistoryLimitChanged(int value) = 0;

    virtual void onStateDockPositionChanged(Qt::DockWidgetArea value) = 0;

    virtual void onStateDockSelectedTabChanged(int value) = 0;

    virtual void onStateWindowSizeChanged(QSize value) = 0;

    virtual void onStateCurrentVariableChanged(int value) = 0;

    virtual void onStateCurrentConstantChanged(int value) = 0;

    virtual void onStateCurrentFunctionChanged(int value) = 0;

    virtual void onStateCurrentScriptChanged(int value) = 0;
};

#endif //QCALC_STATELISTENER_HPP
