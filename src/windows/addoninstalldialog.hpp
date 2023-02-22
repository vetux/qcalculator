/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef QCALCULATOR_ADDONINSTALLDIALOG_HPP
#define QCALCULATOR_ADDONINSTALLDIALOG_HPP

#include <QDialog>
#include <QPushButton>

#include "widgets/installaddonitemwidget.hpp"

class AddonInstallDialog : public QDialog {
Q_OBJECT
public:
    explicit AddonInstallDialog(QWidget *parent = nullptr) : QDialog(parent) {
        updateLabel = new QLabel(this);
        installLabel = new QLabel(this);
        updateListWidget = new QListWidget(this);
        installListWidget = new QListWidget(this);
        okButton = new QPushButton(this);
        cancelButton = new QPushButton(this);

        auto *vlayout = new QVBoxLayout;
        vlayout->addWidget(updateLabel);
        vlayout->addWidget(updateListWidget);
        vlayout->addWidget(installLabel);
        vlayout->addWidget(installListWidget);

        auto *hlayout = new QHBoxLayout;
        hlayout->addStretch(1);
        hlayout->addWidget(okButton);
        hlayout->addWidget(cancelButton);

        vlayout->addLayout(hlayout);

        setLayout(vlayout);

        updateLabel->setText("Addon updates");
        installLabel->setText("New Addons");

        okButton->setText("Install Selected");
        cancelButton->setText("Cancel");

        connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
        connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
    }

    std::vector<std::string> getCheckedItems() {
        std::vector<std::string> ret;
        for (auto &item: items) {
            if (item.second->getChecked()) {
                ret.emplace_back(item.first);
            }
        }
        return ret;
    }

public slots:

    void setUpdates(const std::vector<std::string> &vitems) {
        updateListWidget->setVisible(!vitems.empty());
        updateLabel->setVisible(!vitems.empty());
        updateListWidget->clear();
        for (auto &item: vitems) {
            auto *widget = new InstallAddonItemWidget;
            widget->setText(item.c_str());
            widget->setChecked(true);
            auto *wItem = new QListWidgetItem;
            wItem->setSizeHint(widget->minimumSizeHint());
            updateListWidget->addItem(wItem);
            updateListWidget->setItemWidget(wItem, widget);
            items.insert(std::make_pair(item, widget));
        }
    }

    void setInstalls(const std::vector<std::string> &vitems) {
        installListWidget->setVisible(!vitems.empty());
        installLabel->setVisible(!vitems.empty());
        installListWidget->clear();
        for (auto &item: vitems) {
            auto *widget = new InstallAddonItemWidget;
            widget->setText(item.c_str());
            widget->setChecked(true);
            auto *wItem = new QListWidgetItem;
            wItem->setSizeHint(widget->minimumSizeHint());
            installListWidget->addItem(wItem);
            installListWidget->setItemWidget(wItem, widget);
            items.insert(std::make_pair(item, widget));
        }
    }

private:
    QLabel *updateLabel;
    QListWidget *updateListWidget;
    QLabel *installLabel;
    QListWidget *installListWidget;
    QPushButton *okButton;
    QPushButton *cancelButton;

    std::map<std::string, InstallAddonItemWidget *> items;
};

#endif //QCALCULATOR_ADDONINSTALLDIALOG_HPP
