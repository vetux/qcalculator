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

#include "gui/widgets/scriptseditor.hpp"

#include <QVBoxLayout>
#include <QLabel>

class ScriptItemWidget : public QWidget {
public:
    explicit ScriptItemWidget(QWidget *parent) : QWidget(parent) {
        setLayout(new QHBoxLayout);
        label = new QLabel(this);
        layout()->addWidget(label);
    }

    void setText(const QString &text) {
        label->setText(text);
    }

private:
    QLabel *label;
};

ScriptsEditor::ScriptsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());
    list = new QListWidget(this);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    layout()->addWidget(list);
}

void ScriptsEditor::setScripts(const std::map<std::string, Script> &scripts) {
    list->clear();
    for (auto &p : scripts) {
        auto *widget = new ScriptItemWidget(list);
        auto *item = new QListWidgetItem();

        QString text = p.first.c_str();

        if (p.second.enableArguments)
            text.append(" - Arguments enabled");

        widget->setText(text);

        item->setSizeHint(widget->sizeHint());

        list->addItem(item);
        list->setItemWidget(item, widget);

        list->update();
    }
}
