#ifndef QCALC_SETTINGSDIALOG_HPP
#define QCALC_SETTINGSDIALOG_HPP

#include <QDialog>

#include <set>
#include <string>

#include "addonmetadata.hpp"

#include <mpfr.h>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    ~SettingsDialog() override;

    void setEnabledAddons(const std::set<std::string> &addons);

    std::set<std::string> getEnabledAddons();

    void setPrecision(int precision);

    int getPrecision();

    void setRoundingMode(mpfr_rnd_t rounding);

    mpfr_rnd_t getRoundingMode();

    void setFormattingPrecision(int precision);

    int getFormattingPrecision();

    void setFormattingRoundingMode(mpfr_rnd_t rounding);

    mpfr_rnd_t getFormattingRoundingMode();

    void setSymbolsPrecision(int precision);

    int getSymbolsPrecision();

    void setSymbolsFormattingPrecision(int precision);

    int getSymbolsFormattingPrecision();

private slots:

    void onModuleEnableChanged(bool enabled);

    void onDialogAccepted();

    void onDialogRejected();

    void onSettingsTabChanged(int tab);

    void onRefreshAddonsPressed();

    void onInstallAddonPressed();

private:
    Ui::SettingsDialog *ui;

    std::set<std::string> enabledAddons;

    void applyAddonState(const std::map<std::string, bool> &addonState,
                         const std::map<std::string, AddonMetadata> &addonMetadata);
};

#endif // QCALC_SETTINGSDIALOG_HPP
