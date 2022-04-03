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

#include "addon/addon.hpp"

class AddonTesterDialog : public QDialog {
Q_OBJECT
public:
    explicit AddonTesterDialog(Addon &addon, QWidget *parent = nullptr) : QDialog(parent),
                                                                          addon(addon) {
        setWindowTitle("Addon Tester");
        label = new QLabel(
                "Unloading and loading addon continuously, check for memory leaks in a process manager\nClose dialog to cancel.",
                this);
        resize(250, 150);
        connect(&timer, SIGNAL(timeout()), SLOT(leakCheck()));
    }

    int exec() override {
        auto moduleLoaded = addon.isLoaded();

        if (moduleLoaded)
            addon.unload();

        timer.start();
        int ret = QDialog::exec();

        if (moduleLoaded)
            addon.load();

        return ret;
    }

private slots:

    void leakCheck() {
        addon.load();
        addon.unload();
    }

private:
    Addon &addon;

    QTimer timer;
    QLabel *label;
};

#endif //QCALC_ADDONTESTERDIALOG_HPP
