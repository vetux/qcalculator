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

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>

#include "gui/widgets/historylabel.hpp"

// Because eliding of QLabel widgets is not possible without drawing all the text by hand with qpainter or setting
// the elided text on resize (Contrary to what the qt5 documentation says "https://doc.qt.io/qt-5/qtwidgets-widgets-elidedlabel-example.html"
// which states "QLabel can elide text that doesn't fit within it, but only in one line." which is not true.)
// the history will just have a horizontal scroll bar on overflow for now.
//TODO: Implement custom label which auto ellides text. (This involves using correct style values, drawing the text by hand with qpainter etc.)
class HistoryWidget : public QWidget {
Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setLayout(new QVBoxLayout());
        layout()->setMargin(0);

        scroll = new QScrollArea(this);

        container = new QWidget(this);
        container->setLayout(new QVBoxLayout());
        container->layout()->setMargin(6);
        container->layout()->setSpacing(12);

        scroll->setWidget(container);
        scroll->setWidgetResizable(true);
        scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        layout()->addWidget(scroll);

        container->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

        connect(scroll->verticalScrollBar(),
                SIGNAL(rangeChanged(int, int)),
                this,
                SLOT(onVerticalRangeChanged(int, int)));
    }

public slots:

    void clear() {
        for (auto *p : rows) {
            delete p;
        }
        rows.clear();
    }

    void setContent(const QList<QPair<QString, QString>> &c) {
        clear();
        for (auto &pair : c) {
            addContent(pair.first, pair.second);
        }
    }

    void addContent(const QString &expression, const QString &value) {
        auto *row = new QWidget(container);
        auto *expressionLabel = new HistoryLabel(row);
        auto *equalsLabel = new QLabel(row);
        auto *resultLabel = new HistoryLabel(row);

        expressionLabel->setAlignment(Qt::AlignLeft);
        equalsLabel->setAlignment(Qt::AlignHCenter);
        resultLabel->setAlignment(Qt::AlignRight);

        row->setLayout(new QHBoxLayout());

        row->layout()->addWidget(expressionLabel);
        row->layout()->addWidget(equalsLabel);
        row->layout()->addWidget(resultLabel);

        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        expressionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        equalsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        resultLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        expressionLabel->setTextElided(expression);
        expressionLabel->setText(expression);
        equalsLabel->setText("=");
        resultLabel->setTextElided(value);
        resultLabel->setText(value);

        connect(expressionLabel,
                SIGNAL(onDoubleClick(const QString &)),
                this,
                SIGNAL(onTextDoubleClicked(const QString &)));
        connect(resultLabel,
                SIGNAL(onDoubleClick(const QString &)),
                this,
                SIGNAL(onTextDoubleClicked(const QString &)));

        rows.emplace_back(row);

        container->layout()->addWidget(row);

        // The scrolling has to happen in the rangeChanged callback because its not possible to force update
        // the scrollbars even after invalidating, updating and activating all layouts and widgets involved.
        scrollingToBottom = true;
    }

signals:

    void onTextDoubleClicked(const QString &text);

private slots:

    void onVerticalRangeChanged(int min, int max) {
        if (scrollingToBottom) {
            scroll->verticalScrollBar()->setValue(max);
            scrollingToBottom = false;
        }
    }

private:
    QScrollArea *scroll;
    QWidget *container;
    std::vector<QWidget *> rows;

    bool scrollingToBottom = false;
};

#endif //QCALC_HISTORYWIDGET_HPP
