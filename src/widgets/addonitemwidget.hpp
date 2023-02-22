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

#ifndef QCALC_ADDONITEMWIDGET_HPP
#define QCALC_ADDONITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

class AddonItemWidget : public QWidget {
Q_OBJECT
public:
    explicit AddonItemWidget(QWidget *parent = nullptr);

    void setModuleName(const QString &name);

    QString getModuleName();

    void setModuleEnabled(bool enabled);

    bool getModuleEnabled();

    void setModuleDisplayName(const QString &name);

    void setModuleDescription(const QString &description);

signals:

    void onModuleEnabledChanged(bool enabled);

private:
    QString moduleName;
    QHBoxLayout *layout;
    QCheckBox *checkbox;
    QLabel *label;

private slots:

    void onCheckBoxStateChange(int state);
};

#endif //QCALC_ADDONITEMWIDGET_HPP
