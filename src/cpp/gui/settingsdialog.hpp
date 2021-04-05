#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>

#include <map>

#include "settings.hpp"

#include "addonmetadata.hpp"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    ~SettingsDialog() override;

    void setSettings(const Settings &state);

    Settings getSettings();

private slots:

    void onModuleEnableChanged(bool enabled);

    void onDialogAccepted();

    void onDialogRejected();

    void onResetSettingsPressed();

    void onSettingsTabChanged(int tab);

    void onRefreshAddonsPressed();

    void onInstallAddonPressed();

private:
    Ui::SettingsDialog *ui;

    Settings settings;

    void applyAddonState(const std::map<std::string, bool> &addonState, const std::map<std::string, AddonMetadata>& addonMetadata);
};

#endif // SETTINGSDIALOG_HPP
