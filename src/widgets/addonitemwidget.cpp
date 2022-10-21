/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "widgets/addonitemwidget.hpp"

#include <QPushButton>
#include <QSizePolicy>

#include "addon/addonmanager.hpp"

AddonItemWidget::AddonItemWidget(QWidget *parent) : QWidget(parent),
                                                    layout(nullptr),
                                                    checkbox(nullptr),
                                                    label(nullptr) {
    layout = new QHBoxLayout(this);
    checkbox = new QCheckBox(this);
    label = new QLabel(this);

    checkbox->setStyleSheet("QCheckBox::indicator { width: 18px; height: 18px; }");

    layout->addWidget(checkbox, 0);
    layout->addWidget(label, 1);

    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    label->setContentsMargins(5, 0 ,0, 0);

    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));

    setLayout(layout);
}

void AddonItemWidget::setModuleName(const QString &name) {
    moduleName = name;
}

QString AddonItemWidget::getModuleName() {
    return moduleName;
}

void AddonItemWidget::setModuleEnabled(bool enabled) {
    disconnect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));
    checkbox->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));
}

bool AddonItemWidget::getModuleEnabled() {
    return checkbox->isChecked();
}

void AddonItemWidget::setModuleDisplayName(const QString &name) {
    label->setText(name);
}

void AddonItemWidget::setModuleDescription(const QString &description) {
    label->setToolTip(description);
}

void AddonItemWidget::onCheckBoxStateChange(int state) {
    emit onModuleEnabledChanged(state == Qt::Checked);
}
