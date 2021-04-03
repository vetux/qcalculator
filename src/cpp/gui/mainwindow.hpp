#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <bitset>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>

#include "numeralsystem.hpp"
#include "numpadkey.hpp"

#include "mainview.hpp"
#include "mainpresenter.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * For behaviours which are currently not directly linked to the presenter (Like the QTableWidget logic)
 * the main window binds the widget behaviour to itself and emits
 * custom signals which map to the presenter member functions.
 *
 * In this way there is logic offloaded to the view which is bad.
 *
 * Alternatives are giving the presenter access to the ui pointer (Bad)
 * or making the presenter interface less abstract.
 */
class MainWindow : public QMainWindow, MainView {
Q_OBJECT

signals:

    void signalDecimalSubmit(const QString &value);

    void signalHexSubmit(const QString &value);

    void signalOctalSubmit(const QString &value);

    void signalBinarySubmit(const QString &value);


    void signalNumPadKeyPressed(NumPadKey key);

    void signalBitViewKeyPressed(int bitIndex);


    void signalSelectedVariableChanged(int index);

    void signalVariableChanged(const std::string &name, const std::string &value);


    void signalSelectedConstantChanged(int index);

    void signalConstantChanged(const std::string &name, const std::string &value);


    void signalSelectedFunctionChanged(int index);

    void signalFunctionNameChanged(const std::string &value);

    void signalFunctionBodyChanged(const std::string &value);

    void signalFunctionArgsChanged(const std::vector<std::string> &arguments);

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    //+MainView
    void connectPresenter(const MainPresenter &target) override;

    void disconnectPresenter(const MainPresenter &target) override;

    void showAboutDialog() override;

    bool showSettingsDialog(const SettingsDialogState &input, SettingsDialogState &outputData) override;

    void showWarningDialog(const std::string &title, const std::string &text) override;

    void showInfoDialog(const std::string &title, const std::string &text) override;

    bool showFileChooserDialog(const std::string &title, bool existingFile, std::string &filePathOut) override;

    bool showQuestionDialog(const std::string &title, const std::string &text) override;

    void quit() override;

    void setInputText(const std::string &value) override;

    void setValueText(const std::string &value) override;

    void setKeyPadVisibility(bool visible) override;

    void setBitViewVisibility(bool visible) override;

    void setDockVisibility(bool visible) override;

    void setActiveDockTab(int tab) override;

    void setDockPosition(Qt::DockWidgetArea position) override;

    void setBitViewContents(const std::bitset<64> &value) override;

    void setBitViewEnabled(bool enabled) override;

    void setNumericSystemsEnabled(bool enabled) override;

    void setDecimalText(const std::string &value) override;

    void setHexText(const std::string &value) override;

    void setOctalText(const std::string &value) override;

    void setBinaryText(const std::string &value) override;

    void setHistory(const std::vector<std::pair<std::string, std::string>> &value) override;

    void setVariableListView(const std::vector<std::pair<std::string, std::string>> &value) override;

    void setSelectedVariable(int index) override;

    void setConstantsListView(const std::vector<std::pair<std::string, std::string>> &value) override;

    void setSelectedConstant(int index) override;

    void setFunctionsListView(const std::vector<std::string> &value) override;

    void setSelectedFunction(int index) override;

    void setFunctionArgs(const std::vector<std::string> &value) override;

    void setFunctionBody(const std::string &value) override;

    void setFunctionArgsSpinBoxEnabled(bool enabled) override;

    void setFunctionBodyEnabled(bool enabled) override;

    void setWindowSize(QSize size) override;
    //-MainView

private slots:

    void onNumericSystemsFinishEditing();

    void onNumPadKeyPressed();

    void onBitViewKeyPressed();


    void onVariablesSelectionChanged();

    void onVariablesCellChanged(int row, int column);


    void onConstantsSelectionChanged();

    void onConstantsCellChanged(int row, int column);


    void onFunctionsSelectionChanged();

    void onFunctionsCellChanged(int row, int column);

    void onFunctionsArgCountValueChanged(int value);

    void onFunctionsArgEditingFinished();

    void onFunctionsBodyTextChanged();
    
private:
    Ui::MainWindow *ui;

    MainPresenter presenter;

    //Helpers
    QPushButton &getBitButton(int bitIndex);

    static int getBitIndex(const QString &name);

    QPushButton &getKeyPadButton(NumPadKey key);

    static QString getNameFromKey(NumPadKey key);

    static NumPadKey getKeyFromName(const QString &name);
};

#endif // MAINWINDOW_H
