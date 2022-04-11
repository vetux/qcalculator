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

void AddonTab::setAddons(const std::map<std::string, bool> &addonState,
                         const std::map<std::string, Addon> &addonMetadata) {
    auto searchText = addonSearchEdit->text().toStdString();

    listWidget->clear();
    for (auto &addon: addonState) {
        auto &ad = addonMetadata.at(addon.first);
        if (!searchText.empty()) {
            bool cont = false;
            auto max = addon.first.size() > ad.getDisplayName().size()
                       ? addon.first.size()
                       : ad.getDisplayName().size();
            if (searchText.size() > addon.first.size() && searchText.size() > ad.getDisplayName().size())
                continue;
            for (auto i = 0; i < searchText.size() && i < max; i++) {
                auto c = getCasePair(searchText.at(i));
                if ((i >= addon.first.size() || (addon.first.at(i) != c.first && addon.first.at(i) != c.second))
                    && (i >= ad.getDisplayName().size() ||
                        (ad.getDisplayName().at(i) != c.first && ad.getDisplayName().at(i) != c.second))) {
                    cont = true;
                    break;
                }
            }
            if (cont)
                continue;
        }

        auto *itemWidget = new AddonItemWidget(listWidget);
        itemWidget->setModuleName(addon.first.c_str());
        itemWidget->setModuleEnabled(addon.second);
        itemWidget->setModuleDisplayName(ad.getDisplayName().c_str());
        itemWidget->setModuleDescription(
                (ad.getDescription() + " ( " + addon.first + " )").c_str());

        auto *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->minimumSizeHint());
        listWidget->addItem(item);
        listWidget->setItemWidget(item, itemWidget);

        connect(itemWidget, SIGNAL(onModuleEnabledChanged(bool)), this, SLOT(onAddonEnableChanged()));
        connect(itemWidget, SIGNAL(onUninstallModule(const QString &)), this, SLOT(onModuleUninstall(const QString &)));
    }
    listWidget->update(); // When not calling update here the widget contents are drawn for one frame with smaller size
}

void AddonTab::setLibraries(const std::set<std::string> &libs) {
    auto searchText = libSearchEdit->text().toStdString();

    libListWidget->clear();
    for (auto &lib: libs) {
        if (!searchText.empty()) {
            bool cont = false;
            if (searchText.size() > lib.size()) {
                continue;
            }
            for (int i = 0; i < searchText.size(); i++) {
                auto c = getCasePair(searchText.at(i));
                if (lib.at(i) != c.first && lib.at(i) != c.second) {
                    cont = true;
                    break;
                }
            }
            if (cont)
                continue;
        }

        auto *itemWidget = new LibraryItemWidget(libListWidget);
        itemWidget->setLibrary(lib.c_str());

        auto *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->minimumSizeHint());

        libListWidget->addItem(item);
        libListWidget->setItemWidget(item, itemWidget);

        connect(itemWidget,
                SIGNAL(onUninstallLibrary(const QString &)),
                this,
                SIGNAL(libraryUninstall(const QString &)));
    }
    libListWidget->update(); // When not calling update here the widget contents are drawn for one frame with smaller size
}

AddonTab::AddonTab(QWidget *parent)
        : QWidget(parent) {
    installButton = new QPushButton(this);
    refreshButton = new QPushButton(this);

    listWidget = new QListWidget(this);

    libListWidget = new QListWidget(this);

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

    auto *libHeader = new QWidget();
    auto *libHeaderLayout = new QHBoxLayout();
    libHeaderLayout->setMargin(0);
    libHeader->setLayout(libHeaderLayout);
    libSearchEdit = new QLineEdit();
    libHeaderLayout->addWidget(new QLabel("Libraries"), 1);
    libHeaderLayout->addWidget(new QLabel("Search:"));
    libHeaderLayout->addWidget(libSearchEdit);

    connect(addonSearchEdit,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onAddonSearchTextChanged(const QString &)));

    connect(libSearchEdit,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onLibrarySearchTextChanged(const QString &)));

    auto *header = new QWidget(this);
    header->setLayout(new QHBoxLayout(header));
    header->layout()->setMargin(0);
    header->layout()->addWidget(refreshButton);
    header->layout()->addWidget(installButton);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(addonHeader);
    layout()->addWidget(listWidget);
    layout()->addWidget(libHeader);
    layout()->addWidget(libListWidget);
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

void AddonTab::onLibrarySearchTextChanged(const QString &text) {
    emit refreshPressed();
}
