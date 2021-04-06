#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <bitset>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>

#include "numeralsystem.hpp"
#include "numpadkey.hpp"
#include "addonmanagerlistener.hpp"


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
    void signalInputTextChange(const QString &text);

public slots:

    void onInputTextChanged(const QString &text);

    void onInputReturnPressed();

    void onActionSettings();

    void onActionExit();

    void onActionAbout();

private:
    Ui::MainWindow *ui;

    QString inputText;
};

#endif // MAINWINDOW_H
