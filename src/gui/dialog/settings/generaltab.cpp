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

#include "generaltab.hpp"

#include <QVBoxLayout>
#include <QLine>

int getIndexFromRoundingMode(mpfr_rnd_t mode) {
    switch (mode) {
        default:
        case MPFR_RNDN:
            return 0;
        case MPFR_RNDZ:
            return 1;
        case MPFR_RNDU:
            return 2;
        case MPFR_RNDD:
            return 3;
        case MPFR_RNDA:
            return 4;
    }
}

mpfr_rnd_t getRoundingModeFromIndex(int index) {
    switch (index) {
        default:
        case 0:
            return MPFR_RNDN;
        case 1:
            return MPFR_RNDZ;
        case 2:
            return MPFR_RNDU;
        case 3:
            return MPFR_RNDD;
        case 4:
            return MPFR_RNDA;
    }
}

QFrame *getLine(QWidget *parent = nullptr) {
    //https://stackoverflow.com/a/42139725
    auto *line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

void GeneralTab::setPrecision(int bits) {
    precisionSpinBox->setValue(bits);
}

void GeneralTab::setRounding(mpfr_rnd_t rounding) {
    roundingComboBox->setCurrentIndex(getIndexFromRoundingMode(rounding));
}

void GeneralTab::setFormatPrecision(int digits) {
    formatPrecisionSpinBox->setValue(digits);
}

void GeneralTab::setFormatRounding(mpfr_rnd_t rounding) {
    formatRoundingComboBox->setCurrentIndex(getIndexFromRoundingMode(rounding));
}

GeneralTab::GeneralTab(QWidget *parent)
        : QWidget(parent) {
    roundingModel.setStringList({"Round to nearest",
                                 "Round toward zero",
                                 "Round toward +Inf",
                                 "Round toward -Inf",
                                 "Round away from zero"});
    precisionLabel = new QLabel(this);
    precisionLabel->setText("Precision");
    precisionLabel->setToolTip(
            "The mantissa size in bits of the data type used for literals in expressions. Beware that large values will result in more memory usage and slower computation time.");
    precisionSpinBox = new QSpinBox(this);

    roundingLabel = new QLabel(this);
    roundingLabel->setText("Rounding");
    roundingLabel->setToolTip("The rounding mode to use when doing arithmetic.");
    roundingComboBox = new QComboBox(this);
    roundingComboBox->setModel(&roundingModel);

    formatPrecisionLabel = new QLabel(this);
    formatPrecisionLabel->setText("Format Precision");
    formatPrecisionLabel->setToolTip("The precision in decimal spaces used when formatting result values to strings.");
    formatPrecisionSpinBox = new QSpinBox(this);

    formatRoundingLabel = new QLabel(this);
    formatRoundingLabel->setText("Format Rounding");
    formatRoundingLabel->setToolTip("The rounding mode used when formatting result values to strings.");
    formatRoundingComboBox = new QComboBox(this);

    precisionSpinBox->setRange(1, 1000000000);
    formatPrecisionSpinBox->setRange(0, 1000000);

    formatRoundingComboBox->setModel(&roundingModel);

    auto *layout = new QVBoxLayout();

    layout->addWidget(precisionLabel);
    layout->addWidget(precisionSpinBox);
    layout->addWidget(roundingLabel);
    layout->addWidget(roundingComboBox);

    layout->addSpacing(10);

    layout->addWidget(formatPrecisionLabel);
    layout->addWidget(formatPrecisionSpinBox);
    layout->addWidget(formatRoundingLabel);
    layout->addWidget(formatRoundingComboBox);

    layout->addWidget(new QWidget(this), 1);

    setLayout(layout);
}

int GeneralTab::getPrecision() {
    return precisionSpinBox->value();
}

mpfr_rnd_t GeneralTab::getRounding() {
    return getRoundingModeFromIndex(roundingComboBox->currentIndex());
}

int GeneralTab::getFormatPrecision() {
    return formatPrecisionSpinBox->value();
}

mpfr_rnd_t GeneralTab::getFormatRounding() {
    return getRoundingModeFromIndex(formatRoundingComboBox->currentIndex());
}