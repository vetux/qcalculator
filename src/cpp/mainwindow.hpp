#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "model.hpp" //Has to be the first include because python uses "slots" as a variable name and "slots" is a qt reserved word.

#include <QMainWindow>
#include <bitset>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>

#include "numeralsystem.hpp"
#include "numpadkey.hpp"

#include "view.hpp"
#include "presenter.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//TODO: MVC Rewrite
//TODO: Fix bitview on win32
//TODO: Replace QSettings with custom json settings file (QSettings uses registry on win32 which is unacceptable in my opinion)
class MainWindow : public QMainWindow, View {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    //+View
    void connectPresenter(const Presenter &target) override;

    void disconnectPresenter(const Presenter &target) override;

    void launchAboutDialog() override;

    void launchSettingsDialog() override;

    void quit() override;

    void setInputText(const std::string &value) override;

    void setKeyPadVisibility(bool visible) override;

    void setBitViewVisibility(bool visible) override;

    void setDockVisibility(bool visible) override;

    void setActiveDockTab(int tab) override;

    void setBitViewContents(std::bitset<64> value) override;

    void setBitViewEnabled(bool enabled) override;

    void setNumericSystemsEnabled(bool enabled) override;

    void setDecimalText(const std::string &value) override;

    void setHexText(const std::string &value) override;

    void setOctalText(const std::string &value) override;

    void setBinaryText(const std::string &value) override;

    void setHistory(const std::vector<std::pair<std::string, std::string>> &value) override;

    void setVariableListView(const std::vector<std::pair<std::string, std::string>> &value) override;

    void setActiveVariable(int index) override;

    void setConstantsListView(const std::vector<std::pair<std::string, std::string>> &value) override;

    void setActiveConstant(int index) override;

    void setFunctionsListView(const std::vector<std::string> &value) override;

    void setActiveFunction(int index) override;

    void setFunctionArgs(const std::vector<std::string> &value) override;

    void setFunctionBody(const std::string &value) override;

    void setFunctionArgsEnabled(bool enabled) override;

    void setFunctionBodyEnabled(bool enabled) override;

    void setScriptsListView(const std::vector<std::string> &value) override;

    void setActiveScript(int index) override;

    void setScriptBody(const std::string &value) override;

    void setScriptBodyEnabled(bool enabled) override;

    void setWindowSize(QSize size) override;
    //-View

private:
    Ui::MainWindow *ui;

    Model model;
    Presenter presenter;

    //Helpers
    QPushButton *getBitButton(int bitIndex);

    static int getBitIndex(const QString &name);

    QPushButton *getKeyPadButton(NumPadKey key);

    static QString getNameFromKey(NumPadKey key);

    static NumPadKey getKeyFromName(const QString &name);
};

#endif // MAINWINDOW_H
