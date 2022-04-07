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

#ifndef QCALC_TERMINALWIDGET_HPP
#define QCALC_TERMINALWIDGET_HPP

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

class TerminalWidget : public QWidget {
Q_OBJECT
public:
    explicit TerminalWidget(QWidget *parent = nullptr) : QWidget(parent) {
        historyLabel = new QLabel();
        container = new QWidget();
        promptLabel = new QLabel();
        inputEdit = new QLineEdit();

        container->setLayout(new QHBoxLayout());
        container->layout()->addWidget(promptLabel);
        container->layout()->addWidget(inputEdit);
        container->layout()->setMargin(0);

        promptLabel->setText(">>>");

        historyLabel->setAlignment(Qt::AlignmentFlag::AlignVCenter | Qt::AlignmentFlag::AlignBottom);
        historyLabel->setContentsMargins(0, 0, 0, 0);

        inputEdit->setStyleSheet("QLineEdit { background: none; }");

        connect(inputEdit, SIGNAL(returnPressed()), this, SIGNAL(onReturnPressed()));

        auto l = new QVBoxLayout();
        l->setSpacing(0);
        l->setMargin(0);
        l->addWidget(historyLabel, 1);
        l->addWidget(container);
        setLayout(l);
    }

signals:

    void onReturnPressed();

public slots:

    void appendHistory(const QString &command, const QString &result) {
        historyLabel->setText(historyLabel->text() + "\n>>> " + command + "\n" + result);
        if (historyLabel->text().endsWith("\n"))
            historyLabel->setText(historyLabel->text().chopped(1));
        inputEdit->setText("");
    }

    QString getInputText() {
        return inputEdit->text();
    }

    void setInputText(const QString &text) {
        inputEdit->setText(text);
    }

    QString getPrompt() {
        return promptLabel->text();
    }

    void setPrompt(const QString &text) {
        promptLabel->setText(text);
    }

    QString getHistoryText() {
        return historyLabel->text();
    }

    void setHistoryText(const QString &text) {
        historyLabel->setText(text);
    }

private:
    QLabel *historyLabel;
    QWidget *container;
    QLabel *promptLabel;
    QLineEdit *inputEdit;
};

#endif //QCALC_TERMINALWIDGET_HPP
