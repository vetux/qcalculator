#include <QSettings>

#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include "settings.hpp"

SettingsDialog::SettingsDialog(QSettings &settings, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog),
        settings(settings) {
    ui->setupUi(this);
    ui->groupBox_saverestore->setChecked(settings.value(SETTINGS_SAVE, true).toBool());
    ui->spinBox_historylimit->setValue(settings.value(SETTINGS_HISTORY_LIMIT, 0).toInt());
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::on_pushButton_resetstate_pressed() {
    settings.clear();
    settings.sync();
    ui->groupBox_saverestore->setChecked(settings.value(SETTINGS_SAVE, true).toBool());
    ui->spinBox_historylimit->setValue(settings.value(SETTINGS_HISTORY_LIMIT, 0).toInt());
}

void SettingsDialog::on_spinBox_historylimit_editingFinished() {
    settings.setValue(SETTINGS_HISTORY_LIMIT, ui->spinBox_historylimit->value());
}

void SettingsDialog::on_groupBox_saverestore_toggled(bool arg1)
{
    settings.setValue(SETTINGS_SAVE, arg1);
}
