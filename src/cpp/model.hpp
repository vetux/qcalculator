#ifndef QCALC_MODEL_HPP
#define QCALC_MODEL_HPP

#include "calculatorengine.hpp"

#include "valuetype.hpp"
#include "state.hpp"

typedef std::function<void(State)> StateListener;

class Model {
public:
    Model(StateListener listener);

    void evaluateInput();

    void updateInput(const std::string &input);

    void updateValue(ValueType value);

    void addVariable(Variable var);

    void removeVariable(int index);

    void updateVariable(int index, Variable value);

    void addConstant(Constant value);

    void removeConstant(int index);

    void updateConstant(int index, Constant value);

    void addFunction(Function value);

    void removeFunction(int index);

    void updateFunction(int index, Function value);

    void addScript(Script value);

    void removeScript(int index);

    void updateScript(int index, Script value);

    void updateShowKeyPad(bool value);

    void updateShowBitView(bool value);

    void updateShowDock(bool value);

    void updateHistoryLimit(int limit);

    void updateDockPosition(int position);

    void updateDockSelectedTab(int tab);

    void updateWindowSize(QSize size);

    void updateCurrentVariable(int currentVariable);

    void updateCurrentConstant(int currentConstant);

    void updateCurrentFunction(int currentFunction);

    void updateCurrentScript(int currentScript);

    const State &getState();

private:
    StateListener listener;
    CalculatorEngine engine;
    State state;
};

#endif //QCALC_MODEL_HPP
