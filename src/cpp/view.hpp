#ifndef QCALC_VIEW_HPP
#define QCALC_VIEW_HPP

#include <QObject>
#include <string>
#include <bitset>

class Presenter;

class View {
public:
    //Actions
    virtual void connectPresenter(const Presenter &target) = 0;

    virtual void disconnectPresenter(const Presenter &target) = 0;

    virtual void launchAboutDialog() = 0;

    virtual void launchSettingsDialog() = 0;

    virtual void quit() = 0;

    //UI Update
    virtual void setInputText(const std::string &value) = 0;

    virtual void setKeyPadVisibility(bool visible) = 0;

    virtual void setBitViewVisibility(bool visible) = 0;

    virtual void setDockVisibility(bool visible) = 0;

    virtual void setActiveDockTab(int tab) = 0;

    virtual void setBitViewContents(std::bitset<64> value) = 0;

    virtual void setBitViewEnabled(bool enabled) = 0;

    virtual void setNumericSystemsEnabled(bool enabled) = 0;

    virtual void setDecimalText(const std::string &value) = 0;

    virtual void setHexText(const std::string &value) = 0;

    virtual void setOctalText(const std::string &value) = 0;

    virtual void setBinaryText(const std::string &value) = 0;

    virtual void setHistory(const std::vector<std::pair<std::string, std::string>> &value) = 0;

    virtual void setVariableListView(const std::vector<std::pair<std::string, std::string>> &value) = 0;

    virtual void setActiveVariable(int index) = 0;

    virtual void setConstantsListView(const std::vector<std::pair<std::string, std::string>> &value) = 0;

    virtual void setActiveConstant(int index) = 0;

    virtual void setFunctionsListView(const std::vector<std::string> &value) = 0;

    virtual void setActiveFunction(int index) = 0;

    virtual void setFunctionArgs(const std::vector<std::string> &value) = 0;

    virtual void setFunctionBody(const std::string &value) = 0;

    virtual void setFunctionArgsEnabled(bool enabled) = 0;

    virtual void setFunctionBodyEnabled(bool enabled)= 0;

    virtual void setScriptsListView(const std::vector<std::string> &value) = 0;

    virtual void setActiveScript(int index) = 0;

    virtual void setScriptBody(const std::string &value) = 0;

    virtual void setScriptBodyEnabled(bool enabled) = 0;

    virtual void setWindowSize(QSize size) = 0;
};

#endif //QCALC_VIEW_HPP
