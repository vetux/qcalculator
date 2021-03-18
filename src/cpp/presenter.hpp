#ifndef QCALC_PRESENTER_HPP
#define QCALC_PRESENTER_HPP

#include "model.hpp"

#include <QObject>
#include <QCloseEvent>
#include <QResizeEvent>

#include "numpadkey.hpp"
#include "view.hpp"

class Presenter : public QObject {
Q_OBJECT
public:
    Presenter(Model &model, View &view);

    void init();

    void onStateChange(const State &state);

public slots:

    void onWindowClose(QCloseEvent *event);

    void onWindowResize(QResizeEvent *event);

    void onInputSubmit();

    void onInputUpdate(const QString &value);

    void onDecimalSubmit(QString value);

    void onHexSubmit(QString value);

    void onOctalSubmit(QString value);

    void onBinarySubmit(QString value);

    void onNumPadKeyPressed(NumPadKey key);

    void onBitViewKeyPressed(int bitIndex);

    void onDockVisibilityChanged(bool visible);


    void onCurrentScriptChanged(int index);

    void onScriptNameChanged(std::string value);

    void onScriptBodyChanged(std::string value);


    void onCurrentFunctionChanged(int index);

    void onFunctionNameChanged(std::string value);

    void onFunctionBodyChanged(std::string value);

    void onFunctionArgsChanged(std::vector<std::string> arguments);


    void onCurrentVariableChanged(int index);

    void onVariableChanged(std::string name, std::string value);


    void onCurrentConstantChanged(int index);

    void onConstantChanged(std::string name, std::string value);


    void onActionExit();

    void onActionAbout();

    void onActionSettings();

    void onActionShowKeyPad(bool show);

    void onActionShowBitView(bool show);

    void onActionShowDock(bool show);


    void onDockTabChanged(int tabIndex);

private:
    Model &model;
    View &view;
};

#endif //QCALC_PRESENTER_HPP
