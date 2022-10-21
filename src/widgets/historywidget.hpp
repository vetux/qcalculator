/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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
#include <QCoreApplication>

#include "widgets/historylabel.hpp"

class HistoryWidget : public QWidget {
Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setLayout(new QVBoxLayout());
        layout()->setMargin(0);

        scroll = new QScrollArea(this);

        container = new QWidget(this);
        container->setLayout(new QVBoxLayout());
        container->layout()->setMargin(0);
        container->layout()->setSpacing(12);

        scroll->setWidget(container);
        scroll->setWidgetResizable(true);
        scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        layout()->addWidget(scroll);

        layout()->setMargin(0);

        scroll->setStyleSheet(std::string(
                "QScrollArea{ background: palette(window); border: none; }"
                "QScrollBar:vertical {"
                "    background: palette(window);"
                "    width: 19px;" // <<<<<<<<<<<
                "    margin: 8px 0px 8px 0px;"
                "}"
                "QScrollBar::handle:vertical {"
                "    background: palette(text);"
                "    min-height: 0px;"
                "    margin: 0px 6px 0px 6px;" // <<<<<<<<<<<
                "    width: 7px;" // <<<<<<<<<<<
                "    border-width: 1px;" // <<<<<<<<<<<
                "    border-radius: 2px;" // <<<<<<<<<<<
                "}"
                "QScrollBar::add-line:vertical {"
                "    background: palette(base);"
                "    height: 0px;"
                "    subcontrol-position: bottom;"
                "    subcontrol-origin: margin;"
                "}"
                "QScrollBar::sub-line:vertical {"
                "    background: palette(base);"
                "    height: 0 px;"
                "    subcontrol-position: top;"
                "    subcontrol-origin: margin;"
                "}"
        ).c_str());

        container->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        container->layout()->setContentsMargins(8, 0, 8, 8);
    }

public slots:

    void clear() {
        for (auto *p: rows) {
            delete p;
        }
        rows.clear();
    }

    void setContent(const QList<QPair<QString, QString>> &c) {
        clear();
        for (auto &pair: c) {
            addContent(pair.first, pair.second);
        }
    }

    void addContent(const QString &expression, const QString &value) {
        auto *row = new QWidget(container);
        auto *expressionLabel = new HistoryLabel(row);
        auto *equalsLabel = new QLabel(row);
        auto *resultLabel = new HistoryLabel(row);

        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        expressionLabel->setFont(historyFont);
        equalsLabel->setFont(historyFont);
        resultLabel->setFont(historyFont);

        expressionLabel->setAlignment(Qt::AlignLeft);
        equalsLabel->setAlignment(Qt::AlignHCenter);
        resultLabel->setAlignment(Qt::AlignRight);

        auto *layout = new QHBoxLayout();

        layout->setMargin(0);

        row->setLayout(layout);

        layout->addWidget(expressionLabel, 5);
        layout->addWidget(equalsLabel, 0);
        layout->addWidget(resultLabel, 5);

        container->layout()->addWidget(row);
        container->layout()->invalidate();
        container->layout()->activate();

        // Call to processEvents is required here so that a scroll to the bottom can be done immediately.
        QCoreApplication::processEvents();

        expressionLabel->setText(expression);
        equalsLabel->setText("=");
        resultLabel->setText(value);

        expressionLabel->setCursorPosition(0);
        resultLabel->setCursorPosition(0);

        connect(expressionLabel,
                SIGNAL(onDoubleClick(const QString &)),
                this,
                SIGNAL(onTextDoubleClicked(const QString &)));
        connect(resultLabel,
                SIGNAL(onDoubleClick(const QString &)),
                this,
                SIGNAL(onTextDoubleClicked(const QString &)));

        rows.emplace_back(row);

        scrollToBottom();
    }

    void setHistoryFont(const QFont &font) {
        historyFont = font;
    }

    void scrollToBottom() {
        // Hack to defer scrolling to next event loop iteration https://www.qtcentre.org/threads/18291-Force-a-Scroll-in-a-QScrollArea
        QTimer::singleShot(0, this, [this]() {
            scroll->verticalScrollBar()->setSliderPosition(scroll->verticalScrollBar()->maximum());
        });
    }

signals:

    void onTextDoubleClicked(const QString &text);

private:
    QScrollArea *scroll;
    QWidget *container;
    std::vector<QWidget *> rows;

    QFont historyFont;
};

#endif //QCALC_HISTORYWIDGET_HPP
