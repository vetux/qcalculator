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

#include "widgets/builtinseditor.hpp"

#include <QVBoxLayout>
#include <QLabel>

#include "math/symboltable.hpp"

#include "widgets/scriptseditor.hpp"

BuiltInsEditor::BuiltInsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());
    list = new QListWidget(this);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    checkBox = new QCheckBox(this);
    checkBox->setText("Enable built-in constants (pi, epsilon and inf)");
    layout()->addWidget(checkBox);
    layout()->addWidget(list);
    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

    //Adapted from https://github.com/ArashPartow/exprtk/blob/806c519c91fd08ba4fa19380dbf3f6e42de9e2d1/readme.txt#L161
    setListContents(SymbolTable::getBuiltIns());
}

void BuiltInsEditor::setUseBuiltInConstants(bool useBuiltIns) {
    checkBox->setCheckState(useBuiltIns ? Qt::Checked : Qt::Unchecked);
}

void BuiltInsEditor::setListContents(const std::map<std::string, std::string> &contents) {
    list->clear();
    for (auto &pair: contents) {
        auto *widget = new QWidget(list);
        auto *item = new QListWidgetItem();

        auto *label = new QLabel(widget);
        auto *descLabel = new QLabel(widget);

        label->setMinimumWidth(100);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        label->setText(pair.first.c_str());
        descLabel->setText(pair.second.c_str());

        auto *layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(descLabel, 1);

        widget->setLayout(layout);

        item->setSizeHint(widget->sizeHint());

        list->addItem(item);
        list->setItemWidget(item, widget);

        list->update();
    }
}

void BuiltInsEditor::stateChanged(int state) {
    emit onUseBuiltInConstantChanged(state == Qt::Checked);
}
