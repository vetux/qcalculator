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

    precisionSpinBox->setRange(1, std::numeric_limits<int>().max());

    exponentMinSpinBox->setRange(std::numeric_limits<int>().min(), -1);
    exponentMaxSpinBox->setRange(1, std::numeric_limits<int>().max());

    auto *inexactWarnContainer = new QWidget(this);

    showInexactWarningLabel = new QLabel(this);
    showInexactWarningCheckBox = new QCheckBox(this);
    showInexactWarningLabel->setText("Show inexact warning");
    showInexactWarningLabel->setToolTip("Show Warning when the result of a computation is inexact.");
    showInexactWarningCheckBox->setToolTip("Show Warning when the result of a computation is inexact.");

    pythonModPathLabel = new QLabel(this);
    pythonModPathListWidget = new QListWidget(this);
    pythonModPathAddPushButton = new QPushButton(this);

    pythonModPathListWidget->setToolTip("List of paths that are added to the python module search path");

    pythonModPathLabel->setText("Python Module Paths");
    pythonModPathAddPushButton->setText("Add Directory");

    pythonPathLabel = new QLabel(this);
    pythonPathEdit = new QLineEdit(this);

    pythonPathLabel->setText("Python Default Path");

    auto *hlayout = new QHBoxLayout;
    hlayout->setSpacing(20);
    hlayout->addWidget(showInexactWarningLabel, 0);
    hlayout->addWidget(showInexactWarningCheckBox, 1);

    inexactWarnContainer->setLayout(hlayout);

    hlayout = new QHBoxLayout;
    hlayout->addWidget(pythonModPathLabel, 1);
    hlayout->addWidget(pythonModPathAddPushButton, 0);

    hlayout->setMargin(0);
    hlayout->setContentsMargins(5, 10, 5, 0);

    auto *pythonModPathContainerWidget = new QWidget(this);
    pythonModPathContainerWidget->setLayout(hlayout);

    pythonModPathContainerWidget->setToolTip(
            "List of paths that are added to the python module search path on application startup");

    auto *layout = new QVBoxLayout();

    layout->addWidget(precisionLabel);
    layout->addWidget(precisionSpinBox);
    layout->addWidget(exponentMaxLabel);
    layout->addWidget(exponentMaxSpinBox);
    layout->addWidget(exponentMinLabel);
    layout->addWidget(exponentMinSpinBox);
    layout->addWidget(roundingLabel);
    layout->addWidget(roundingComboBox);
    layout->addWidget(inexactWarnContainer);
    layout->addSpacing(10);
    layout->addWidget(pythonPathLabel);
    layout->addWidget(pythonPathEdit);
    layout->addWidget(pythonModPathContainerWidget);
    layout->addWidget(pythonModPathListWidget, 1);

    setLayout(layout);

    connect(pythonModPathAddPushButton, SIGNAL(pressed()), this, SLOT(addModPathPressed()));
}

int GeneralTab::getPrecision() {
    return precisionSpinBox->value();
}

decimal::round GeneralTab::getRounding() {
    return getRoundingModeFromIndex(roundingComboBox->currentIndex());
}

void GeneralTab::setShowInexactWarning(bool showWarning) {
    showInexactWarningCheckBox->setCheckState(showWarning ? Qt::Checked : Qt::Unchecked);
}

bool GeneralTab::getShowInexactWarning() {
    return showInexactWarningCheckBox->checkState() == Qt::Checked;
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

void GeneralTab::setPythonModPaths(const std::set<std::string> &paths) {
    pythonModPathListWidget->clear();
    for (auto &path: paths) {
        addItem(path.c_str());
    }
}

std::set<std::string> GeneralTab::getPythonModPaths() {
    std::set<std::string> ret;
    for (auto i = 0; i < pythonModPathListWidget->count(); i++) {
        auto *item = pythonModPathListWidget->item(i);
        auto *widget = dynamic_cast<StringEditItemWidget *>(pythonModPathListWidget->itemWidget(item));
        ret.insert(widget->getText().toStdString());
    }
    return ret;
}

void GeneralTab::setPythonPath(const std::string &path) {
    pythonPathEdit->setText(path.c_str());
}

std::string GeneralTab::getPythonPath() {
    return pythonPathEdit->text().toStdString();
}

void GeneralTab::addModPathPressed() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    for (auto &path: fileNames) {
        addItem(path);
    }
}

void GeneralTab::addItem(const QString &path) {
    auto *itemWidget = new StringEditItemWidget(pythonModPathListWidget);
    itemWidget->setReadOnly(true);
    itemWidget->setDeletable(true);
    itemWidget->setText(path);

    connect(itemWidget, SIGNAL(deleteString(const QString &)), this, SLOT(removeItem(const QString &)));

    auto *item = new QListWidgetItem();
    item->setSizeHint(itemWidget->minimumSizeHint());

    pythonModPathListWidget->addItem(item);
    pythonModPathListWidget->setItemWidget(item, itemWidget);
}

void GeneralTab::removeItem(const QString &path) {
    for (auto i = 0; i < pythonModPathListWidget->count(); i++) {
        auto *item = pythonModPathListWidget->item(i);
        auto *itemWidget = dynamic_cast<StringEditItemWidget *>(pythonModPathListWidget->itemWidget(item));

        if (itemWidget->getText() == path) {
            pythonModPathListWidget->removeItemWidget(item);
            pythonModPathListWidget->takeItem(i);
        }
    }
}
