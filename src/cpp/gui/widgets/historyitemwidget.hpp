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

#ifndef QCALC_HISTORYITEMWIDGET_HPP
#define QCALC_HISTORYITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class HistoryItemWidget : public QWidget {
Q_OBJECT
public:
    explicit HistoryItemWidget(QWidget *parent = nullptr) : QWidget(parent) {
        layout = new QHBoxLayout(this);

        equalsText = new QLabel(this);

        expressionText = new QLabel(this);
        resultText = new QLabel(this);

        equalsText->setTextInteractionFlags(Qt::TextSelectableByMouse);

        expressionText->setTextInteractionFlags(Qt::TextSelectableByMouse);
        resultText->setTextInteractionFlags(Qt::TextSelectableByMouse);

        equalsText->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        equalsText->setAlignment(Qt::AlignCenter);

        resultText->setAlignment(Qt::AlignRight);
        resultText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        expressionText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        layout->addWidget(expressionText, 6);
        layout->addWidget(equalsText, 1);
        layout->addWidget(resultText, 3);

        equalsText->setText("=");
    }

    void resizeEvent(QResizeEvent *event) override {
        updateText();
    }

    void setContents(const QString &e, const QString &r) {
        expression = e;
        result = r;
        updateText();
    }

    void updateText() {
        expressionText->setText("");
        resultText->setText("");
        layout->activate();

        QFontMetrics metrics = QFontMetrics(expressionText->font());

        QString elitext = metrics.elidedText(expression, Qt::ElideRight, expressionText->width());
        expressionText->setText(elitext);
        if (expression != elitext)
            expressionText->setToolTip(expression);
        else
            expressionText->setToolTip("");

        elitext = metrics.elidedText(result, Qt::ElideRight, resultText->width());
        resultText->setText(elitext);
        if (result != elitext)
            resultText->setToolTip(result);
        else
            resultText->setToolTip("");
    }

private:
    QString expression;
    QString result;
    QHBoxLayout *layout;
    QLabel *equalsText;
    QLabel *expressionText;
    QLabel *resultText;
};

#endif //QCALC_HISTORYITEMWIDGET_HPP
