#include <QSettings>

#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
//    ui->checkBox->setChecked(settings.value(SETTINGS_SAVE, true).toBool());
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::on_pushButton_resetstate_pressed() {
  /*  settings.clear();
    settings.sync();
    ui->checkBox->setChecked(settings.value(SETTINGS_SAVE, true).toBool());*/
}

void SettingsDialog::on_checkBox_toggled(bool arg1) {
 //   settings.setValue(SETTINGS_SAVE, arg1);
}
