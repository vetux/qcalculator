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

#include "widgets/generaltab.hpp"

#include <QVBoxLayout>
#include <QLine>
#include <QFileDialog>

int getIndexFromRoundingMode(decimal::round mode) {
    switch (mode) {
        default:
        case MPD_ROUND_UP:
            return 0;
        case MPD_ROUND_DOWN:
            return 1;
        case MPD_ROUND_CEILING:
            return 2;
        case MPD_ROUND_FLOOR:
            return 3;
        case MPD_ROUND_HALF_UP:
            return 4;
        case MPD_ROUND_HALF_DOWN:
            return 5;
        case MPD_ROUND_HALF_EVEN:
            return 6;
        case MPD_ROUND_05UP:
            return 7;
        case MPD_ROUND_TRUNC:
            return 8;
    }
}

decimal::round getRoundingModeFromIndex(int index) {
    int ret;
    switch (index) {
        default:
        case 0:
            ret = MPD_ROUND_UP;
            break;
        case 1:
            ret = MPD_ROUND_DOWN;
            break;
        case 2:
            ret = MPD_ROUND_CEILING;
            break;
        case 3:
            ret = MPD_ROUND_FLOOR;
            break;
        case 4:
            ret = MPD_ROUND_HALF_UP;
            break;
        case 5:
            ret = MPD_ROUND_HALF_DOWN;
            break;
        case 6:
            ret = MPD_ROUND_HALF_EVEN;
            break;
        case 7:
            ret = MPD_ROUND_05UP;
            break;
        case 8:
            ret = MPD_ROUND_TRUNC;
            break;
    }
    return (decimal::round) ret;
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

void GeneralTab::setRounding(decimal::round rounding) {
    roundingComboBox->setCurrentIndex(getIndexFromRoundingMode(rounding));
}

GeneralTab::GeneralTab(QWidget *parent)
        : QWidget(parent) {
    roundingModel.setStringList({
                                        "Round away from 0",
                                        "Round toward 0 (truncate)",
                                        "Round toward +infinity",
                                        "Round toward -infinity",
                                        "Round 0.5 up",
                                        "Round 0.5 down",
                                        "Round 0.5 to even",
                                        "Round zero or five away from 0",
                                        "Truncate, but set infinity"
                                });

    precisionLabel = new QLabel(this);
    precisionSpinBox = new QSpinBox(this);
    precisionLabel->setText("Precision");
    precisionLabel->setToolTip(
            "The number of digits of precision. Beware that large values will result in more memory usage and slower computation time.");
    precisionSpinBox->setToolTip(
            "The number of digits of precision. Beware that large values will result in more memory usage and slower computation time.");

    exponentMaxLabel = new QLabel(this);
    exponentMaxSpinBox = new QSpinBox(this);
    exponentMaxLabel->setText("Exponent Maximum");
    exponentMaxLabel->setToolTip(
            "The maximum value of *the* exponent. Beware that large values will result in more memory usage and slower computation time.");
    exponentMaxSpinBox->setToolTip(
            "The maximum value of *the* exponent. Beware that large values will result in more memory usage and slower computation time.");

    exponentMinLabel = new QLabel(this);
    exponentMinSpinBox = new QSpinBox(this);
    exponentMinLabel->setText("Exponent Minimum");
    exponentMinLabel->setToolTip(
            "The minimum value of *the* exponent. Beware that large values will result in more memory usage and slower computation time.");
    exponentMinSpinBox->setToolTip(
            "The minimum value of *the* exponent. Beware that large values will result in more memory usage and slower computation time.");

    roundingLabel = new QLabel(this);
    roundingComboBox = new QComboBox(this);
    roundingLabel->setText("Rounding");
    roundingLabel->setToolTip("The rounding mode to use when doing arithmetic.");
    roundingComboBox->setToolTip("The rounding mode to use when doing arithmetic.");
    roundingComboBox->setModel(&roundingModel);

    precisionSpinBox->setRange(1, std::numeric_limits<int>::max());

    exponentMinSpinBox->setRange(std::numeric_limits<int>::min(), -1);
    exponentMaxSpinBox->setRange(1, std::numeric_limits<int>::max());

    saveHistoryLabel = new QLabel(this);
    saveHistoryCheckBox = new QCheckBox(this);
    saveHistoryLabel->setText("Save history to disk");

    clearResultLabel = new QLabel(this);
    clearResultCheckBox = new QCheckBox(this);
    clearResultLabel->setText("Clear result when typing");

    loadRecentSymbolsLabel = new QLabel(this);
    loadRecentSymbolsCheckBox = new QCheckBox(this);
    loadRecentSymbolsLabel->setText("Load the most recently opened symbol table on startup");

    auto *hlayout = new QHBoxLayout;
    hlayout->setMargin(5);
    hlayout->setSpacing(20);
    hlayout->addWidget(saveHistoryCheckBox, 0);
    hlayout->addWidget(saveHistoryLabel, 1);

    auto *saveHistoryContainer = new QWidget(this);

    saveHistoryContainer->setLayout(hlayout);

    hlayout = new QHBoxLayout;
    hlayout->setMargin(5);
    hlayout->setSpacing(20);
    hlayout->addWidget(clearResultCheckBox, 0);
    hlayout->addWidget(clearResultLabel, 1);

    auto *clearResultContainer = new QWidget(this);

    clearResultContainer->setLayout(hlayout);

    hlayout = new QHBoxLayout;
    hlayout->setMargin(5);
    hlayout->setSpacing(20);
    hlayout->addWidget(loadRecentSymbolsCheckBox, 0);
    hlayout->addWidget(loadRecentSymbolsLabel, 1);

    auto *loadRecentSymbolsContainer = new QWidget(this);

    loadRecentSymbolsContainer->setLayout(hlayout);

    auto *layout = new QVBoxLayout();

    layout->addWidget(precisionLabel);
    layout->addWidget(precisionSpinBox);
    layout->addWidget(exponentMaxLabel);
    layout->addWidget(exponentMaxSpinBox);
    layout->addWidget(exponentMinLabel);
    layout->addWidget(exponentMinSpinBox);
    layout->addWidget(roundingLabel);
    layout->addWidget(roundingComboBox);
    layout->addWidget(saveHistoryContainer);
    layout->addWidget(clearResultContainer);
    layout->addWidget(loadRecentSymbolsContainer);
    layout->addStretch(1);

    setLayout(layout);
}

int GeneralTab::getPrecision() {
    return precisionSpinBox->value();
}

decimal::round GeneralTab::getRounding() {
    return getRoundingModeFromIndex(roundingComboBox->currentIndex());
}

void GeneralTab::setExponentMax(int max) {
    exponentMaxSpinBox->setValue(max);
}

void GeneralTab::setExponentMin(int min) {
    exponentMinSpinBox->setValue(min);
}

int GeneralTab::getExponentMax() {
    return exponentMaxSpinBox->value();
}

int GeneralTab::getExponentMin() {
    return exponentMinSpinBox->value();
}

void GeneralTab::setSaveHistory(bool saveHistory) {
    saveHistoryCheckBox->setCheckState(saveHistory ? Qt::Checked : Qt::Unchecked);
}

bool GeneralTab::getSaveHistory() {
    return saveHistoryCheckBox->checkState() == Qt::Checked;
}


void GeneralTab::setClearResult(bool clearResult) {
    clearResultCheckBox->setCheckState(clearResult ? Qt::Checked : Qt::Unchecked);
}

bool GeneralTab::getClearResult() {
    return clearResultCheckBox->checkState() == Qt::Checked;
}

void GeneralTab::setLoadRecentSymbols(bool load) {
    loadRecentSymbolsCheckBox->setCheckState(load ? Qt::Checked : Qt::Unchecked);
}

bool GeneralTab::getLoadRecentSymbols() {
    return loadRecentSymbolsCheckBox->checkState() == Qt::Checked;
}
