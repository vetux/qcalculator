#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>

#include <map>

#include "gui/settingsdialogstate.hpp"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    ~SettingsDialog() override;

    void setDialogState(const SettingsDialogState &state);

    SettingsDialogState getDialogState();

private slots:

    void onModuleEnableChanged(bool enabled);

    void onDialogAccepted();

    void onDialogRejected();

private:
    Ui::SettingsDialog *ui;

    std::map<std::string, bool> addonState; // The names of available addon modules and their enable state.
};

#endif // SETTINGSDIALOG_HPP
