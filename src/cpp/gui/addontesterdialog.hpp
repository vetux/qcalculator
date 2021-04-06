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
        setWindowTitle("AddonHelper Tester");
        label = new QLabel("Running leak check...\nClose dialog to cancel.", this);
        resize(250, 150);
        connect(&timer, SIGNAL(timeout()), SLOT(leakCheck()));
        modulesToBeTested.insert("sample_gui");
        modulesToBeTested.insert("sample_sym");
        timer.start();
    }

private slots:

    void leakCheck() {
        for (auto &module : modulesToBeTested) {
            AddonHelper::load(module);
            AddonHelper::unload(module);
        }
    }

private:
    std::set<std::string> modulesToBeTested;

    QTimer timer;

    QLabel *label;
};

#endif //QCALC_ADDONTESTERDIALOG_HPP
