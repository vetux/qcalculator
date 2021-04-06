#ifndef QCALC_ADDONTESTERDIALOG_HPP
#define QCALC_ADDONTESTERDIALOG_HPP

#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QCoreApplication>

#include <set>
#include <string>

#include "addonhelper.hpp"

class AddonTesterDialog : public QDialog {
Q_OBJECT
public:
    explicit AddonTesterDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Addon Tester");
        label = new QLabel("Running memory leak check...\nClose dialog to cancel.", this);
        resize(250, 150);
        connect(&timer, SIGNAL(timeout()), SLOT(leakCheck()));
    }

    void setModule(const std::string &m, bool isLoaded) {
        module = m;
        moduleLoaded = isLoaded;
    }

    int exec() override {
        if (moduleLoaded)
            AddonHelper::unload(module);
        timer.start();
        int ret = QDialog::exec();
        if (moduleLoaded)
            AddonHelper::load(module);
        return ret;
    }

private slots:

    void leakCheck() {
        AddonHelper::load(module);
        AddonHelper::unload(module);
    }

private:
    std::string module;
    bool moduleLoaded;

    QTimer timer;
    QLabel *label;
};

#endif //QCALC_ADDONTESTERDIALOG_HPP
