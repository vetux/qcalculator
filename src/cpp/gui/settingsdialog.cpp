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

void SettingsDialog::setSettings(const Settings &s) {
    settings = s;

    std::map<std::string, AddonMetadata> metadata = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());

    std::map<std::string, bool> addonState;
    for (auto &pair : metadata) {
        addonState[pair.first] = settings.enabledAddonModules.find(pair.first) != settings.enabledAddonModules.end();
    }

    applyAddonState(addonState, metadata);

    ui->tabWidget->setCurrentIndex(settings.settingsTab);
}

Settings SettingsDialog::getSettings() {
    return settings;
}

void SettingsDialog::onModuleEnableChanged(bool enabled) {
    auto &s = dynamic_cast<AddonItemWidget &>(*sender());

    bool load = true;
    if (enabled && settings.showAddonWarning) {
        auto *checkBox = new QCheckBox();
        checkBox->setText("Dont show this dialog again");

        QMessageBox messageBox;
        messageBox.setWindowTitle("Addons Information");
        messageBox.setText(
                "Make sure to verify the addon source code before enabling it. Running an addon is equivalent to running any other python script. Do you want to enable the addon?");
        messageBox.setIcon(QMessageBox::Icon::Question);
        messageBox.addButton(QMessageBox::Ok);
        messageBox.addButton(QMessageBox::Cancel);
        messageBox.setDefaultButton(QMessageBox::Cancel);
        messageBox.setCheckBox(checkBox);

        messageBox.exec();

        load = messageBox.result() == QMessageBox::Ok;

        settings.showAddonWarning = checkBox->isChecked();
    }

    if (load) {
        std::string name = s.getModuleName().toStdString();

        auto it = settings.enabledAddonModules.find(name);
        if (it != settings.enabledAddonModules.end()) {
            if (!enabled) {
                settings.enabledAddonModules.erase(name);
            }
        } else {
            if (enabled) {
                settings.enabledAddonModules.insert(name);
            }
        }
    } else {
        s.setModuleEnabled(false);
    }
}

void SettingsDialog::onDialogAccepted() {
    accept();
}

void SettingsDialog::onDialogRejected() {
    reject();
}

void SettingsDialog::onResetSettingsPressed() {
    settings = {};

    std::map<std::string, AddonMetadata> metadata = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());
    std::map<std::string, bool> addonState;
    for (auto &pair : metadata) {
        addonState[pair.first] = settings.enabledAddonModules.find(pair.first) != settings.enabledAddonModules.end();
    }

    applyAddonState(addonState, metadata);
}

void SettingsDialog::onSettingsTabChanged(int tab) {
    settings.settingsTab = tab;
}

void SettingsDialog::onRefreshAddonsPressed() {
    std::map<std::string, AddonMetadata> metadata = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());
    std::map<std::string, bool> addonState;
    for (auto &pair : metadata) {
        addonState[pair.first] = settings.enabledAddonModules.find(pair.first) != settings.enabledAddonModules.end();
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
