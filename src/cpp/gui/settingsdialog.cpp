#include "ui_settingsdialog.h"

#include <QSettings>
#include <QMessageBox>

#include "gui/settingsdialog.hpp"

#include "gui/widgets/addonitemwidget.hpp"

#include "addonhelper.hpp"
#include "paths.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onDialogAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onDialogRejected()));
    connect(ui->pushButton_resetSettings, SIGNAL(pressed()), this, SLOT(onResetSettingsPressed()));
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

void SettingsDialog::onResetSettingsPressed() {
    enabledAddons = {};

    std::map<std::string, AddonMetadata> metadata = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());
    std::map<std::string, bool> addonState;
    for (auto &pair : metadata) {
        addonState[pair.first] = false;
    }

    applyAddonState(addonState, metadata);
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
