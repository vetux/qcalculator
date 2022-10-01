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

#ifndef QCALCULATOR_PYTHONTAB_HPP
#define QCALCULATOR_PYTHONTAB_HPP

#include <QWidget>

#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QStringListModel>
#include <QCheckBox>
#include <QListWidget>

#include <set>

#include "gui/widgets/stringedititemwidget.hpp"
#include "math/arithmetictype.hpp"

class PythonTab : public QWidget {
Q_OBJECT
public slots:
    void setPythonModPaths(const std::set<std::string> &paths);

    void setPythonPath(const std::string &path);

public:
    explicit PythonTab(QWidget *parent = nullptr);

    std::set<std::string> getPythonModPaths();

    std::string getPythonPath();

private slots:
    void addModPathPressed();

    void addItem(const QString &path);

    void removeItem(const QString &path);

private:
    QLabel *pythonPathLabel;
    QLineEdit *pythonPathEdit;

    QLabel *pythonModPathLabel;
    QPushButton *pythonModPathAddPushButton;
    QListWidget *pythonModPathListWidget;
};

#endif //QCALCULATOR_PYTHONTAB_HPP
