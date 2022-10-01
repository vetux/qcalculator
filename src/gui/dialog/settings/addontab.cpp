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

#include <QLineEdit>

#include "gui/widgets/addonitemwidget.hpp"

std::pair<char, char> getCasePair(char c) {
    if (c >= 65 && c <= 90) {
        return {c, c + 32};
    } else if (c >= 97 && c <= 122) {
        return {c - 32, c};
    } else {
        return {c, c};
    }
}

void AddonTab::setAddons(const std::map<std::string, Addon> &addons) {
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
        connect(itemWidget, SIGNAL(onUninstallModule(const QString &)), this, SLOT(onModuleUninstall(const QString &)));
    }
    listWidget->update(); // When not calling update here the widget contents are drawn for one frame with smaller size
}

AddonTab::AddonTab(QWidget *parent)
        : QWidget(parent) {
    installButton = new QPushButton(this);
    refreshButton = new QPushButton(this);

    listWidget = new QListWidget(this);

    installButton->setText("Install");
    refreshButton->setText("Refresh");

    auto *addonHeader = new QWidget();
    auto *addonHeaderLayout = new QHBoxLayout();
    addonHeaderLayout->setMargin(0);
    addonHeader->setLayout(addonHeaderLayout);
    addonSearchEdit = new QLineEdit();
    addonHeaderLayout->addWidget(new QLabel("Addons"), 1);
    addonHeaderLayout->addWidget(new QLabel("Search:"));
    addonHeaderLayout->addWidget(addonSearchEdit);

    connect(addonSearchEdit,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onAddonSearchTextChanged(const QString &)));

    auto *header = new QWidget(this);
    header->setLayout(new QHBoxLayout(header));
    header->layout()->setMargin(0);
    header->layout()->addWidget(refreshButton);
    header->layout()->addWidget(installButton);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(addonHeader);
    layout()->addWidget(listWidget);
    layout()->addWidget(header);

    connect(installButton, SIGNAL(pressed()), this, SIGNAL(installPressed()));
    connect(refreshButton, SIGNAL(pressed()), this, SIGNAL(refreshPressed()));
}


void AddonTab::onAddonEnableChanged() {
    emit addonEnableChanged(dynamic_cast<AddonItemWidget *>(sender()));
}

void AddonTab::onModuleUninstall(const QString &module) {
    emit addonUninstall(module);
}

void AddonTab::onAddonSearchTextChanged(const QString &text) {
    emit refreshPressed();
}
