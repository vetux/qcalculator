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

#ifndef QCALCULATOR_ADDONWIDGET_HPP
#define QCALCULATOR_ADDONWIDGET_HPP

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "addon/addon.hpp"

class AddonWidget : public QWidget {
Q_OBJECT
public:
    AddonWidget(QWidget *parent = nullptr) : QWidget(parent) {
        addonTitleLabel = new QLabel(this);
        addonDescriptionLabel = new QLabel(this);

        addonDescriptionLabel->setWordWrap(true);
        addonDescriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        addonTitleLabel->setWordWrap(true);

        auto *vLayout = new QVBoxLayout();
        vLayout->addWidget(addonTitleLabel);
        vLayout->addWidget(addonDescriptionLabel);
        vLayout->addStretch(1);

        setLayout(vLayout);
    }

    void setAddon(const Addon &addon) {
        addonTitleLabel->setText(addon.getDisplayName().c_str());
        addonDescriptionLabel->setText(addon.getDescription().c_str());
    }

private:
    QLabel *addonTitleLabel;
    QLabel *addonDescriptionLabel;
};

#endif //QCALCULATOR_ADDONWIDGET_HPP
