#ifndef QCALC_MAINVIEW_HPP
#define QCALC_MAINVIEW_HPP

#include <QObject>
#include <string>
#include <bitset>

#include "settings.hpp"

class MainPresenter;

class MainView {
public:
    virtual void connectPresenter(const MainPresenter &target) = 0;

    virtual void disconnectPresenter(const MainPresenter &target) = 0;

    virtual void showAboutDialog() = 0;

    virtual bool showSettingsDialog(Settings &settings) = 0;

    virtual void showWarningDialog(const std::string &title, const std::string &text) = 0;

    virtual void showInfoDialog(const std::string &title, const std::string &text) = 0;

    virtual bool showFileChooserDialog(const std::string &title, bool existingFile, std::string &filePathOut) = 0;

    virtual bool showQuestionDialog(const std::string &title, const std::string &text) = 0;

    virtual void quit() = 0;


    virtual void setInputText(const std::string &value) = 0;

    virtual void setValueText(const std::string &value) = 0;

    virtual void setKeyPadVisibility(bool visible) = 0;

    virtual void setBitViewVisibility(bool visible) = 0;

    virtual void setActiveTab(int tab) = 0;

    virtual void setBitViewContents(const std::bitset<64> &value) = 0;

    virtual void setBitViewEnabled(bool enabled) = 0;

    virtual void setNumericSystemsEnabled(bool enabled) = 0;

    virtual void setDecimalText(const std::string &value) = 0;

    virtual void setHexText(const std::string &value) = 0;

    virtual void setOctalText(const std::string &value) = 0;

    virtual void setBinaryText(const std::string &value) = 0;

    virtual void setHistory(const std::vector<std::pair<std::string, std::string>> &value) = 0;

    virtual void setVariableListView(const std::vector<std::pair<std::string, std::string>> &value) = 0;

    virtual void setSelectedVariable(int index) = 0;

    virtual void setConstantsListView(const std::vector<std::pair<std::string, std::string>> &value) = 0;

    virtual void setSelectedConstant(int index) = 0;

    virtual void setFunctionsListView(const std::vector<std::string> &value) = 0;

    virtual void setSelectedFunction(int index) = 0;

    virtual void setFunctionArgs(const std::vector<std::string> &value) = 0;

    virtual void setFunctionBody(const std::string &value) = 0;

    virtual void setFunctionArgsSpinBoxEnabled(bool enabled) = 0;

    virtual void setFunctionBodyEnabled(bool enabled) = 0;

    virtual void setWindowSize(QSize size) = 0;
};

#endif //QCALC_MAINVIEW_HPP
