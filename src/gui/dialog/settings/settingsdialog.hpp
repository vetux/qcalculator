/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_SETTINGSDIALOG_HPP
#define QCALC_SETTINGSDIALOG_HPP

#include <QDialog>

#include <set>
#include <string>

#include <mpfr.h>

#include "../../../addon/addonmetadata.hpp"

#include "generaltab.hpp"
#include "addontab.hpp"

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

    void onModuleEnableChanged(AddonItemWidget *item);

    void onDialogAccepted();

    void onDialogRejected();

    void onSettingsTabChanged(int tab);

    void onRefreshAddonsPressed();

    void onInstallAddonPressed();

private:
    QPushButton *okButton;
    QPushButton *cancelButton;

    QTabWidget *tabWidget;

    GeneralTab *generalTab;
    AddonTab *addonTab;

    std::set<std::string> enabledAddons;;
};

#endif // QCALC_SETTINGSDIALOG_HPP
