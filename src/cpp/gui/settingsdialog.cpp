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
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setDialogState(const SettingsDialogState &state) {
    ui->listWidget_addons->clear();
    addonState = state.addonState;
    for (auto &addon : addonState) {
        auto *itemWidget = new AddonItemWidget(ui->listWidget_addons);
        itemWidget->setModuleName(addon.first.c_str());
        itemWidget->setModuleEnabled(addon.second);
        itemWidget->setModuleDisplayName(state.addonMetadata.at(addon.first).displayName.c_str());
        itemWidget->setModuleDescription(state.addonMetadata.at(addon.first).description.c_str());

        auto *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->minimumSizeHint());
        ui->listWidget_addons->addItem(item);
        ui->listWidget_addons->setItemWidget(item, itemWidget);
        connect(itemWidget, SIGNAL(onModuleEnabledChanged(bool)), this, SLOT(onModuleEnableChanged(bool)));
    }
}

SettingsDialogState SettingsDialog::getDialogState() {
    SettingsDialogState ret;
    ret.addonState = addonState;
    return ret;
}

void SettingsDialog::onModuleEnableChanged(bool enabled) {
    auto &s = dynamic_cast<AddonItemWidget &>(*sender());
    std::string name = s.getModuleName().toStdString();
    addonState.at(name) = enabled;
}

void SettingsDialog::onDialogAccepted() {
    accept();
}

void SettingsDialog::onDialogRejected() {
    reject();
}
