#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calculatorengine.hpp" //Has to be the first include because python uses "slots" as a variable name and "slots" is a qt reserved word.

#include <QMainWindow>
#include <bitset>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>

#include "numeralsystem.hpp"
#include "numpadkey.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//TODO: MVC Rewrite
class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private slots:

    void slotInputReturnPressed();

    void slotInputDecimalEditingFinished();

    void slotInputHexEditingFinished();

    void slotInputOctalEditingFinished();

    void slotInputBinaryEditingFinished();

    void slotNumPadClick();

    void slotBitClick();

    void slot_dockWidget_visibilityChanged(bool visible);

    void slot_tableWidget_scripts_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void slot_tableWidget_scripts_itemChanged(QTableWidgetItem *item);

    void slot_textEdit_scripts_textChanged();

    void slotFunctionTableWidgetCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void slotFunctionTableWidgetItemChanged(QTableWidgetItem *item);

    void slotFunctionArgCountChange(int args);

    void slotFunctionArgChange0(const QString &arg);

    void slotFunctionArgChange1(const QString &arg);

    void slotFunctionArgChange2(const QString &arg);

    void slotFunctionArgChange3(const QString &arg);

    void slotFunctionArgChange4(const QString &arg);

    void slotFunctionBodyChange();

    void slotVariablesTableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void slotVariablesTableWidget_itemChanged(QTableWidgetItem *item);

    void slotConstantsTableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void slotConstantsTableWidget_itemChanged(QTableWidgetItem *item);

    void on_actionExit_triggered();

    void on_actionAbout_Qt_Calculator_triggered();

    void on_actionShow_Keypad_toggled(bool arg1);

    void on_actionShow_Bit_Toggle_toggled(bool arg1);

    void on_actionShow_Dock_toggled(bool arg1);

    void on_actionSettings_triggered();

private:
    Ui::MainWindow *ui;
    QSettings settings;

    CalculatorEngine<long double> calc;
    CalculatorEngine<long double>::SymbolTable symbols;

    NumeralSystem inputLiteralParseMode = NumeralSystem::DECIMAL;

    std::vector<std::pair<std::string, long double>> expressionHistory;

    std::map<QTableWidgetItem *, std::string> scriptMapping;
    QTableWidgetItem *currentScript = nullptr;
    std::map<QTableWidgetItem *, std::string> functionMapping;
    QTableWidgetItem *currentFunction = nullptr;
    std::map<QTableWidgetItem *, std::string> variableMapping;
    std::map<QTableWidgetItem *, std::string> constantsMapping;

    std::bitset<64> bitViewValue;

    void connectUI();

    void disconnectUI();

    bool parseExpression(const std::string &expr);

    //Ui control
    void setBitView(std::bitset<64> value);

    void hideBitView();

    void setInput(const QString &s);

    void setInput(const std::string &s);

    void setInput(long double value);

    void setToolboxValues(long double value);

    void setHistory(std::vector<std::pair<std::string, long double>> history);

    //Events
    void onInputCommit(const std::string &input);

    void onToolboxCommit(const std::string &input);

    void onKeyPadInput(NumPadKey key);

    void onNumeralSystemChange(NumeralSystem system);

    void onBitViewCommit(std::bitset<64> value);

    //Helpers
    QPushButton *getBitButton(int bitIndex);

    static int getBitIndex(const QString &name);

    QPushButton *getKeyPadButton(NumPadKey key);

    static QString getNameFromKey(NumPadKey key);

    static NumPadKey getKeyFromName(const QString &name);

    static std::string getInputFromkey(NumPadKey key);

    void loadState();

    void saveState();
};

#endif // MAINWINDOW_H
