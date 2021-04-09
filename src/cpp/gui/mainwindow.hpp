#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <bitset>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>

#include <set>

#include "numeralsystem.hpp"
#include "numpadkey.hpp"
#include "addonmanagerlistener.hpp"
#include "settings.hpp"

#include "calc/symboltable.hpp"

#include "gui/widgets/symbolseditor.hpp"
#include "gui/widgets/historywidget.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * MainWindow is Presenter.
 *
 * QT Widgets are View.
 *
 * The main window is now just a simple container which only provides a single line edit for expression parser input.
 *
 * All other features will be implemented via addons.
 *
 * The main window will not reference the native ui elements after the constructor has run.
 *
 * The linkage between the native ui and the main window are now just signals and slots which can be changed
 * from python.
 */
class MainWindow : public QMainWindow, public AddonManagerListener {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void onAddonLoadFail(const std::string &moduleName, const std::string &error) override;

    void onAddonUnloadFail(const std::string &moduleName, const std::string &error) override;

signals:

    /**
     * Emitted whenever the input text for the main window changed.
     * This is emitted after evaluating an expression and is per default connected to the line edit set text slot.
     *
     * @param text
     */
    void signalInputTextChange(const QString &text);

    /**
     * Emitted whenever a expression was evaluated by the main window.
     * This is emitted after evaluating an expression and is per default connected to the history widget addContent slot.
     *
     * pair.first is the expression and pair.second is the result in decimal string form.
     *
     * @param expression
     * @param value
     */
    void signalExpressionEvaluated(const QString &expression, const QString &value);

public slots:

    /**
     * This slot evaluates the input text when invoked.
     */
    void onInputReturnPressed();

    void onSymbolTableChanged(const SymbolTable &symbolTable);

    void onActionSettings();

    void onActionExit();

    void onActionAbout();

    void onActionImportSymbolTable();

    void onActionExportSymbolTable();

private:
    void exitRoutine();

    Ui::MainWindow *ui;

    HistoryWidget *history;

    SymbolsEditor *symbolsEditor;

    SymbolTable symbolTable;

    Settings settings;
};

#endif // MAINWINDOW_H
