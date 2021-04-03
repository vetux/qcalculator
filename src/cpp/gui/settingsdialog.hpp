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

    void onResetSettingsPressed();

    void onSettingsTabChanged(int tab);

private:
    Ui::SettingsDialog *ui;

    SettingsDialogState state;

    void applyAddonState(const std::map<std::string, bool> &addonState, const std::map<std::string, AddonMetadata>& addonMetadata);
};

#endif // SETTINGSDIALOG_HPP
