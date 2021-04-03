#ifndef QCALC_ADDONTESTER_HPP
#define QCALC_ADDONTESTER_HPP

#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QCoreApplication>

#include <set>
#include <string>

#include "addon.hpp"

class AddonTester : public QDialog {
Q_OBJECT
public:
    explicit AddonTester(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Addon Tester");
        label = new QLabel("Running leak check...\nClose dialog to cancel.", this);
        connect(&timer, SIGNAL(timeout()), SLOT(leakCheck()));
        modulesToBeTested.insert("sample_gui");
        modulesToBeTested.insert("sample_sym");
        timer.start();
    }

private slots:

    void leakCheck() {
        for (auto &module : modulesToBeTested) {
            Addon::load(module);
            Addon::unload(module);
        }
    }

private:
    std::set<std::string> modulesToBeTested;

    QTimer timer;

    QLabel *label;
};

#endif //QCALC_ADDONTESTER_HPP
