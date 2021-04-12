#include "gui/settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include <QSettings>
#include <QMessageBox>

#include "gui/widgets/addonitemwidget.hpp"

#include "addonhelper.hpp"
#include "paths.hpp"

int getIndexFromRoundingMode(mpfr_rnd_t mode) {
    switch (mode) {
        default:
        case MPFR_RNDN:
            return 0;
        case MPFR_RNDZ:
            return 1;
        case MPFR_RNDU:
            return 2;
        case MPFR_RNDD:
            return 3;
        case MPFR_RNDA:
            return 4;
    }
}

mpfr_rnd_t getRoundingModeFromIndex(int index) {
    switch (index) {
        default:
        case 0:
            return MPFR_RNDN;
        case 1:
            return MPFR_RNDZ;
        case 2:
            return MPFR_RNDU;
        case 3:
            return MPFR_RNDD;
        case 4:
            return MPFR_RNDA;
    }
}

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onDialogAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onDialogRejected()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onSettingsTabChanged(int)));
    connect(ui->pushButton_installAddon, SIGNAL(pressed()), this, SLOT(onInstallAddonPressed()));
    connect(ui->pushButton_refreshAddons, SIGNAL(pressed()), this, SLOT(onRefreshAddonsPressed()));
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setEnabledAddons(const std::set<std::string> &addons) {
    enabledAddons = addons;

    std::map<std::string, AddonMetadata> metadata = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());

    std::map<std::string, bool> addonState;
    for (auto &pair : metadata) {
        addonState[pair.first] = enabledAddons.find(pair.first) != enabledAddons.end();
    }

    applyAddonState(addonState, metadata);
}

std::set<std::string> SettingsDialog::getEnabledAddons() {
    return enabledAddons;
}

void SettingsDialog::setPrecision(int precision) {
    ui->spinBox_precision->setValue(precision);
}

int SettingsDialog::getPrecision() {
    return ui->spinBox_precision->value();
}

void SettingsDialog::setRoundingMode(mpfr_rnd_t rounding) {
    ui->comboBox_rounding->setCurrentIndex(getIndexFromRoundingMode(rounding));
}

mpfr_rnd_t SettingsDialog::getRoundingMode() {
    return getRoundingModeFromIndex(ui->comboBox_rounding->currentIndex());
}

void SettingsDialog::setFormattingPrecision(int precision) {
    ui->spinBox_formatting_precision->setValue(precision);
}

int SettingsDialog::getFormattingPrecision() {
    return ui->spinBox_formatting_precision->value();
}

void SettingsDialog::setFormattingRoundingMode(mpfr_rnd_t rounding) {
    ui->comboBox_formatting_rounding->setCurrentIndex(getIndexFromRoundingMode(rounding));
}

mpfr_rnd_t SettingsDialog::getFormattingRoundingMode() {
    return getRoundingModeFromIndex(ui->comboBox_formatting_rounding->currentIndex());
}

void SettingsDialog::setSymbolsPrecision(int precision) {
    ui->spinBox_symbols_precision->setValue(precision);
}

int SettingsDialog::getSymbolsPrecision() {
    return ui->spinBox_symbols_precision->value();
}

void SettingsDialog::onModuleEnableChanged(bool enabled) {
    auto &s = dynamic_cast<AddonItemWidget &>(*sender());

    std::string name = s.getModuleName().toStdString();

    auto it = enabledAddons.find(name);
    if (it != enabledAddons.end()) {
        if (!enabled) {
            enabledAddons.erase(name);
        }
    } else {
        if (enabled) {
            enabledAddons.insert(name);
        }
    }
}

void SettingsDialog::onDialogAccepted() {
    accept();
}

void SettingsDialog::onDialogRejected() {
    reject();
}

void SettingsDialog::onSettingsTabChanged(int tab) {
}

void SettingsDialog::onRefreshAddonsPressed() {
    std::map<std::string, AddonMetadata> metadata = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());
    std::map<std::string, bool> addonState;
    for (auto &pair : metadata) {
        addonState[pair.first] = enabledAddons.find(pair.first) != enabledAddons.end();
    }

    applyAddonState(addonState, metadata);
}

void SettingsDialog::onInstallAddonPressed() {
}

void SettingsDialog::applyAddonState(const std::map<std::string, bool> &addonState,
                                     const std::map<std::string, AddonMetadata> &addonMetadata) {
    ui->listWidget_addons->clear();
    for (auto &addon : addonState) {
        auto *itemWidget = new AddonItemWidget(ui->listWidget_addons);
        itemWidget->setModuleName(addon.first.c_str());
        itemWidget->setModuleEnabled(addon.second);
        itemWidget->setModuleDisplayName(addonMetadata.at(addon.first).displayName.c_str());
        itemWidget->setModuleDescription(
                (addonMetadata.at(addon.first).description + " ( " + addon.first + " )").c_str());

        auto *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->minimumSizeHint());
        ui->listWidget_addons->addItem(item);
        ui->listWidget_addons->setItemWidget(item, itemWidget);
        connect(itemWidget, SIGNAL(onModuleEnabledChanged(bool)), this, SLOT(onModuleEnableChanged(bool)));
    }
}
