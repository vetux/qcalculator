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

#ifndef QCALC_NAMEDVALUEEDITOR_HPP
#define QCALC_NAMEDVALUEEDITOR_HPP

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>

#include <map>

class NamedValueEditor : public QWidget {
Q_OBJECT
public:
    explicit NamedValueEditor(QWidget *parent = nullptr);

public slots:

    void setValues(const std::map<QString, QString> &values);

signals:

    void onNamedValueAdded(const QString &name, const QString &value);

    void onNameChanged(const QString &originalName, const QString &name);

    void onValueChanged(const QString &name, const QString &value);

private slots:
    void onAddPressed();

    void onTableCellChanged(int row, int column);

private:
    QTableWidget *list;
    QLineEdit *addLineEditName;
    QLineEdit *addLineEditValue;
    QPushButton *addPushButton;

    std::map<int, std::string> mapping;
};

#endif //QCALC_NAMEDVALUEEDITOR_HPP
