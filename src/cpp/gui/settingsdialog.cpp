#include "ui_settingsdialog.h"

#include <QSettings>

#include "gui/settingsdialog.hpp"

#include "gui/widgets/addonitemwidget.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onDialogAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onDialogRejected()));
    connect(ui->pushButton_resetSettings, SIGNAL(pressed()), this, SLOT(onResetSettingsPressed()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onSettingsTabChanged(int)));
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setDialogState(const SettingsDialogState &s) {
    state = s;
    std::map<std::string, bool> addonState;
    for (auto &pair : state.addonMetadata) {
        addonState[pair.first] = state.settings.enabledAddonModules.find(pair.first) != state.settings.enabledAddonModules.end();
    }
    applyAddonState(addonState, state.addonMetadata);
    ui->tabWidget->setCurrentIndex(state.settings.settingsTab);
}

SettingsDialogState SettingsDialog::getDialogState() {
    return state;
}

void SettingsDialog::onModuleEnableChanged(bool enabled) {
    auto &s = dynamic_cast<AddonItemWidget &>(*sender());
    std::string name = s.getModuleName().toStdString();

    auto it = state.settings.enabledAddonModules.find(name);
    if (it != state.settings.enabledAddonModules.end()) {
        if (!enabled) {
            state.settings.enabledAddonModules.erase(name);
        }
    } else {
        if (enabled) {
            state.settings.enabledAddonModules.insert(name);
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
    state.settings = {};

    std::map<std::string, bool> addonState;
    for (auto &pair : state.addonMetadata) {
        addonState[pair.first] = state.settings.enabledAddonModules.find(pair.first) != state.settings.enabledAddonModules.end();
    }
    applyAddonState(addonState, state.addonMetadata);
}

void SettingsDialog::onSettingsTabChanged(int tab) {
    state.settings.settingsTab = tab;
}

void SettingsDialog::applyAddonState(const std::map<std::string, bool> &addonState,
                                     const std::map<std::string, AddonMetadata> &addonMetadata) {
    ui->listWidget_addons->clear();
    for (auto &addon : addonState) {
        auto *itemWidget = new AddonItemWidget(ui->listWidget_addons);
        itemWidget->setModuleName(addon.first.c_str());
        itemWidget->setModuleEnabled(addon.second);
        itemWidget->setModuleDisplayName(addonMetadata.at(addon.first).displayName.c_str());
        itemWidget->setModuleDescription(addonMetadata.at(addon.first).description.c_str());

        auto *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->minimumSizeHint());
        ui->listWidget_addons->addItem(item);
        ui->listWidget_addons->setItemWidget(item, itemWidget);
        connect(itemWidget, SIGNAL(onModuleEnabledChanged(bool)), this, SLOT(onModuleEnableChanged(bool)));
    }
}
