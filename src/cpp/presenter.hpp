#ifndef QCALC_PRESENTER_HPP
#define QCALC_PRESENTER_HPP

#include "model.hpp"

#include <QObject>
#include <QCloseEvent>
#include <QResizeEvent>

#include "numpadkey.hpp"
#include "view.hpp"
#include "statelistener.hpp"

class Presenter : public QObject, public StateListener {
Q_OBJECT
public:
    Presenter(Model &model, View &view);

    void init();

    void onStateValueChanged(ValueType value) override;

    void onStateHistoryChanged(const std::vector<std::pair<std::string, ValueType>> &value) override;

    void onStateInputChanged(const std::string &value) override;

    void onStateSymbolTableChanged(const SymbolTable &value) override;

    void onStateShowKeyPadChanged(bool value) override;

    void onStateShowBitViewChanged(bool value) override;

    void onStateShowDockChanged(bool value) override;

    void onStateHistoryLimitChanged(int value) override;

    void onStateDockPositionChanged(Qt::DockWidgetArea value) override;

    void onStateDockSelectedTabChanged(int value) override;

    void onStateWindowSizeChanged(QSize value) override;

    void onStateCurrentVariableChanged(int value) override;

    void onStateCurrentConstantChanged(int value) override;

    void onStateCurrentFunctionChanged(int value) override;

    void onStateCurrentScriptChanged(int value) override;

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
    //-Presenter Interface

private:
    Model &model;
    View &view;
};

#endif //QCALC_PRESENTER_HPP
