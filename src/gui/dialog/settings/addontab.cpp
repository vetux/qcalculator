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

#include "addontab.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QFileDialog>

#include <filesystem>
#include <fstream>

#include "gui/widgets/addonitemwidget.hpp"

#include "addon/addonmanager.hpp"

std::pair<char, char> getCasePair(char c) {
    if (c >= 65 && c <= 90) {
        return {c, c + 32};
    } else if (c >= 97 && c <= 122) {
        return {c - 32, c};
    } else {
        return {c, c};
    }
}

void AddonTab::setAddons(const std::map<std::string, Addon> &add) {
    addons = add;
    auto searchText = addonSearchEdit->text().toStdString();
    listWidget->clear();
    for (auto &addon: addons) {
        auto displayName = addon.second.getDisplayName();

        if (!searchText.empty()) {
            auto moduleName = addon.second.getModuleName();

            if (searchText.size() > moduleName.size() && searchText.size() > displayName.size())
                continue;

            bool cont = false;
            auto max = moduleName.size() > displayName.size() ? moduleName.size() : displayName.size();
            for (auto i = 0; i < searchText.size() && i < max; i++) {
                auto c = getCasePair(searchText.at(i));
                if ((i >= moduleName.size() || (moduleName.at(i) != c.first && moduleName.at(i) != c.second))
                    && (i >= displayName.size() || (displayName.at(i) != c.first && displayName.at(i) != c.second))) {
                    cont = true;
                    break;
                }
            }
            if (cont)
                continue;
        }

        auto *itemWidget = new AddonItemWidget(listWidget);
        itemWidget->setModuleName(addon.first.c_str());
        itemWidget->setModuleEnabled(addon.second.isLoaded());
        itemWidget->setModuleDisplayName(displayName.c_str());
        itemWidget->setModuleDescription((addon.second.getDescription() + " ( " + addon.first + " )").c_str());

        auto *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->minimumSizeHint());
        listWidget->addItem(item);
        listWidget->setItemWidget(item, itemWidget);

        connect(itemWidget, SIGNAL(onModuleEnabledChanged(bool)), this, SLOT(onAddonEnableChanged()));
    }
    listWidget->update(); // When not calling update here the widget contents are drawn for one frame with smaller size
}

AddonTab::AddonTab(QWidget *parent)
        : QWidget(parent) {
    installButton = new QPushButton(this);
    refreshButton = new QPushButton(this);
    uninstallButton = new QPushButton(this);

    listWidget = new QListWidget(this);

    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    installButton->setText("Install");
    refreshButton->setText("Refresh");
    uninstallButton->setText("Uninstall");

    auto *addonHeaderLayout = new QHBoxLayout();
    addonHeaderLayout->setMargin(0);

    addonSearchEdit = new QLineEdit();
    addonHeaderLayout->addWidget(new QLabel("Search:"));
    addonHeaderLayout->addWidget(addonSearchEdit);

    connect(addonSearchEdit,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onAddonSearchTextChanged(const QString &)));

    auto btnsLayout = new QHBoxLayout;

    btnsLayout->setMargin(0);
    btnsLayout->addWidget(refreshButton, 2);
    btnsLayout->addWidget(installButton, 2);

    addonDetail = new QWidget(this);

    addonWidget = new AddonWidget(this);

    auto *vLayout = new QVBoxLayout;
    vLayout->addWidget(addonWidget);
    vLayout->addWidget(uninstallButton);

    vLayout->setMargin(0);

    addonDetail->setLayout(vLayout);

    auto *hLayout = new QHBoxLayout();
    hLayout->addWidget(listWidget, 3);
    hLayout->addWidget(addonDetail, 1);

    auto *layout = new QVBoxLayout();
    layout->addLayout(addonHeaderLayout);
    layout->addLayout(hLayout);
    layout->addLayout(btnsLayout);

    setLayout(layout);

    connect(installButton, SIGNAL(clicked()), this, SIGNAL(installPressed()));
    connect(refreshButton, SIGNAL(clicked()), this, SIGNAL(refreshPressed()));
    connect(uninstallButton, SIGNAL(clicked()), this, SLOT(uninstallPressed()));
    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(listItemChange()));

    auto sp = addonDetail->sizePolicy();
    sp.setRetainSizeWhenHidden(false);
    addonDetail->setSizePolicy(sp);

    addonDetail->hide();
}


void AddonTab::onAddonEnableChanged() {
    emit addonEnableChanged(dynamic_cast<AddonItemWidget *>(sender()));
}

void AddonTab::onModuleUninstall(const QString &module) {
    emit addonUninstall(module);
}

void AddonTab::onAddonSearchTextChanged(const QString &text) {
    setAddons(addons);
}

void AddonTab::listItemChange() {
    if (!listWidget->selectedItems().empty()) {
        auto *widget = dynamic_cast<AddonItemWidget *>(listWidget->itemWidget(listWidget->selectedItems().at(0)));
        selectedAddon = widget->getModuleName().toStdString();
        addonWidget->setAddon(addons.at(selectedAddon));
        addonWidget->show();
        listWidget->doItemsLayout();
        listWidget->update();
        addonDetail->show();
    } else {
        selectedAddon.clear();
        addonDetail->hide();
    }
}

void AddonTab::uninstallPressed() {
    emit onModuleUninstall(selectedAddon.c_str());
}
