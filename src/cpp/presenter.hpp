#ifndef QCALC_PRESENTER_HPP
#define QCALC_PRESENTER_HPP

#include "calculatorengine.hpp"

#include <QObject>
#include <QCloseEvent>
#include <QResizeEvent>

#include "numpadkey.hpp"
#include "view.hpp"
#include "settings.hpp"
#include "history.hpp"

class Presenter : public QObject {
Q_OBJECT
public:
    explicit Presenter(View &view);

    void init();

public slots:

    //+Presenter Interface
    void onWindowClose(const QCloseEvent &event);

    void onWindowResize(const QResizeEvent &event);


    void onInputSubmit();

    void onInputUpdate(const QString &value);


    void onDecimalSubmit(const QString &value);

    void onHexSubmit(const QString &value);

    void onOctalSubmit(const QString &value);

    void onBinarySubmit(const QString &value);


    void onNumPadKeyPressed(NumPadKey key);

    void onBitViewKeyPressed(int bitIndex);


    void onSelectedVariableChanged(int index);

    void onVariableChanged(const std::string &name, const std::string &value);


    void onSelectedConstantChanged(int index);

    void onConstantChanged(const std::string &name, const std::string &value);


    void onSelectedFunctionChanged(int index);

    void onFunctionNameChanged(const std::string &value);

    void onFunctionBodyChanged(const std::string &value);

    void onFunctionArgsChanged(const std::vector<std::string> &arguments);


    void onSelectedScriptChanged(int index);

    void onScriptNameChanged(const std::string &value);

    void onScriptBodyChanged(const std::string &value);

    void onScriptEnableArgsChanged(bool value);


    void onActionExit();

    void onActionAbout();

    void onActionSettings();

    void onActionShowKeyPad(bool show);

    void onActionShowBitView(bool show);

    void onActionShowDock(bool show);

    void onActionImportSymbolTable();

    void onActionExportSymbolTable();


    void onDockTabChanged(int tabIndex);

    void onDockVisibilityChanged(bool visible);

    void onDockPositionChanged(Qt::DockWidgetArea area);
    //-Presenter Interface

private:
    View &view;

    CalculatorEngine calculatorEngine;
    SymbolTable symbolTable;
    Settings settings;

    History history;

    ValueType currentValue;

    std::string inputText;

    int currentVariable = -1;
    int currentConstant = -1;
    int currentFunction = -1;
    int currentScript = -1;

    void applyCurrentValue();

    void applySymbolTable();

    void applyVariables();

    void applyConstants();

    void applyFunctions();

    void applyCurrentFunction();

    void applyScripts();

    void applyCurrentScript();
};

#endif //QCALC_PRESENTER_HPP
