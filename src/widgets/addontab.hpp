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

#ifndef QCALC_ADDONTAB_HPP
#define QCALC_ADDONTAB_HPP

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>

#include <set>

#include "widgets/addonitemwidget.hpp"
#include "widgets/addonwidget.hpp"

#include "addon/addon.hpp"

class AddonTab : public QWidget {
Q_OBJECT
signals:

    void addonEnableChanged(AddonItemWidget *item);

    void addonUninstall(const QString &module);

    void installPressed();

    void refreshPressed();

public slots:

    void setAddons(const std::map<std::string, Addon> &addonMetadata);

public:
    explicit AddonTab(QWidget *parent = nullptr);

private slots:
    void uninstallPressed();

    void onAddonEnableChanged();

    void onModuleUninstall(const QString &name);

    void listItemChange();

    void onAddonSearchTextChanged(const QString &text);

private:
    std::map<std::string, Addon> addons;
    std::string selectedAddon;

    QWidget *addonDetail;
    QPushButton *installButton;
    QPushButton *refreshButton;
    QPushButton *uninstallButton;
    QListWidget *listWidget;
    QLineEdit *addonSearchEdit;
    AddonWidget *addonWidget;
};

#endif //QCALC_ADDONTAB_HPP
