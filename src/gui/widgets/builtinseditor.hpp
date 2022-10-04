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

#ifndef QCALCULATOR_BUILTINSEDITOR_HPP
#define QCALCULATOR_BUILTINSEDITOR_HPP

#include <set>

#include <QWidget>
#include <QListWidget>
#include <QCheckBox>

class BuiltInsEditor : public QWidget {
Q_OBJECT
public:
    explicit BuiltInsEditor(QWidget *parent = nullptr);

public slots:

    void setUseBuiltInConstants(bool useBuiltIns);

signals:

    void onUseBuiltInConstantChanged(bool useBuiltIns);

private slots:
    void stateChanged(int state);

private:
    void setListContents(const std::vector<std::pair<QString, QString>> &contents);

    QCheckBox *checkBox;
    QListWidget *list;
};

#endif //QCALCULATOR_BUILTINSEDITOR_HPP
