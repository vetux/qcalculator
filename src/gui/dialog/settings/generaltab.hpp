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

#ifndef QCALC_GENERALTAB_HPP
#define QCALC_GENERALTAB_HPP

#include <QWidget>

#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QStringListModel>

#include <mpfr.h>

class GeneralTab : public QWidget {
Q_OBJECT
public slots:

    void setPrecision(int bits);

    void setRounding(mpfr_rnd_t rounding);

    void setFormatPrecision(int digits);

    void setFormatRounding(mpfr_rnd_t rounding);

public:
    explicit GeneralTab(QWidget *parent = nullptr);

    int getPrecision();

    mpfr_rnd_t getRounding();

    int getFormatPrecision();

    mpfr_rnd_t getFormatRounding();

private:
    QStringListModel roundingModel;

    QLabel *precisionLabel;
    QSpinBox *precisionSpinBox;

    QLabel *roundingLabel;
    QComboBox *roundingComboBox;

    QLabel *formatPrecisionLabel;
    QSpinBox *formatPrecisionSpinBox;

    QLabel *formatRoundingLabel;
    QComboBox *formatRoundingComboBox;
};

#endif //QCALC_GENERALTAB_HPP
