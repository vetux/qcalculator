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

#ifndef QCALC_HISTORYWIDGET_HPP
#define QCALC_HISTORYWIDGET_HPP

#include <QListWidget>

#include "gui/widgets/historyitemwidget.hpp"

//TODO: Fix history overlapping scrollbar when reducing window size with different tab active.
class HistoryWidget : public QListWidget {
Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setSelectionMode(QAbstractItemView::NoSelection);
        setVerticalScrollMode(ScrollPerPixel);
        setFocusPolicy(Qt::NoFocus);
    }

public slots:

    void setContent(const QList<QPair<QString, QString>> &c) {
        clear();
        for (auto &pair : c) {
            addContent(pair.first, pair.second);
        }
    }

    void addContent(const QString &expression, const QString &value) {
        auto *widget = new HistoryItemWidget(this);
        auto *item = new QListWidgetItem(this);

        widget->setContents(expression, value);

        item->setSizeHint(widget->sizeHint());

        addItem(item);
        setItemWidget(item, widget);

        update();

        scrollToBottom();
    }
};

#endif //QCALC_HISTORYWIDGET_HPP
