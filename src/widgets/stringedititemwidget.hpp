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

#ifndef QCALCULATOR_DELETEABLESTRINGITEMWIDGET_HPP
#define QCALCULATOR_DELETEABLESTRINGITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

class StringEditItemWidget : public QWidget {
Q_OBJECT
public:
    explicit StringEditItemWidget(QWidget *parent = nullptr) : QWidget(parent) {
        layout = new QHBoxLayout();
        lineEdit = new QLineEdit();
        buttonDelete = new QPushButton();
        buttonDelete->setText("Delete");
        layout->addWidget(lineEdit, 1);
        layout->addWidget(buttonDelete, 0);
        setLayout(layout);

        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
        connect(buttonDelete, SIGNAL(clicked()), this, SLOT(stringDelete()));
    }

    void setReadOnly(bool readOnly) {
        lineEdit->setReadOnly(readOnly);
    }

    void setDeletable(bool deletable) {
        buttonDelete->setVisible(deletable);
    }

    void setText(const std::string &str) {
        lineEdit->setText(str.c_str());
    }

    void setText(const QString &str) {
        lineEdit->setText(str);
    }

    QString getText() {
        return lineEdit->text();
    }

signals:

    void editedString(const QString &value);

    void deleteString(const QString &value);

private:
    QHBoxLayout *layout;

    QLineEdit *lineEdit;
    QPushButton *buttonDelete;

private slots:

    void editingFinished() {
        emit editedString(lineEdit->text());
    }

    void stringDelete() {
        emit deleteString(lineEdit->text());
    }
};

#endif //QCALCULATOR_DELETEABLESTRINGITEMWIDGET_HPP
