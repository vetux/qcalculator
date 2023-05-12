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

#ifndef QCALC_GENERALTAB_HPP
#define QCALC_GENERALTAB_HPP

#include <QWidget>

#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QStringListModel>
#include <QCheckBox>
#include <QListWidget>

#include <set>

#include <decimal.hh>

#include "widgets/stringedititemwidget.hpp"

class GeneralTab : public QWidget {
Q_OBJECT
public slots:

    void setPrecision(int bits);

    void setRounding(decimal::round rounding);

    void setExponentMax(int max);

    void setExponentMin(int min);

    void setSaveHistory(bool saveHistory);

    void setClearResult(bool clearResult);

    void setLoadRecentSymbols(bool load);

public:
    explicit GeneralTab(QWidget *parent = nullptr);

    int getPrecision();

    decimal::round getRounding();

    int getExponentMax();

    int getExponentMin();

    bool getSaveHistory();

    bool getClearResult();

    bool getLoadRecentSymbols();

private:
    QStringListModel roundingModel;

    QLabel *precisionLabel;
    QSpinBox *precisionSpinBox;

    QLabel *exponentMinLabel;
    QSpinBox *exponentMinSpinBox;

    QLabel *exponentMaxLabel;
    QSpinBox *exponentMaxSpinBox;

    QLabel *roundingLabel;
    QComboBox *roundingComboBox;

    QLabel *saveHistoryLabel;
    QCheckBox *saveHistoryCheckBox;

    QLabel *clearResultLabel;
    QCheckBox *clearResultCheckBox;

    QLabel *loadRecentSymbolsLabel;
    QCheckBox *loadRecentSymbolsCheckBox;
};

#endif //QCALC_GENERALTAB_HPP
