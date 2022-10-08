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

#ifndef QCALCULATOR_INSTALLADDONITEMWIDGET_HPP
#define QCALCULATOR_INSTALLADDONITEMWIDGET_HPP

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QHBoxLayout>

class InstallAddonItemWidget : public QWidget {
Q_OBJECT
public:
    InstallAddonItemWidget(QWidget *parent = nullptr) : QWidget(parent) {
        label = new QLabel(this);
        checkBox = new QCheckBox(this);
        auto *l = new QHBoxLayout(this);
        l->addWidget(label, 1);
        l->addWidget(checkBox);
        setLayout(l);
    }

    bool getChecked() {
        return checkBox->checkState() == Qt::Checked;
    }

public slots:

    void setText(const QString &str) {
        label->setText(str);
    }

    void setChecked(bool checked) {
        checkBox->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }

private:
    QLabel *label;
    QCheckBox *checkBox;
};

#endif //QCALCULATOR_INSTALLADDONITEMWIDGET_HPP
