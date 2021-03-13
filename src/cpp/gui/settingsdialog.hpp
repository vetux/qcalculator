#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(QSettings &settings, QWidget *parent = nullptr);

    ~SettingsDialog() override;

private slots:

    void on_pushButton_resetstate_pressed();

    void on_checkBox_toggled(bool arg1);

private:
    Ui::SettingsDialog *ui;
    QSettings &settings;
};

#endif // SETTINGSDIALOG_HPP
