/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_ADDONTESTERDIALOG_HPP
#define QCALC_ADDONTESTERDIALOG_HPP

#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QCoreApplication>

#include <set>
#include <string>

#include "../../addon/addonhelper.hpp"

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
