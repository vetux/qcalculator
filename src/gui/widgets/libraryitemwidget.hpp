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

#ifndef QCALC_LIBRARYITEMWIDGET_HPP
#define QCALC_LIBRARYITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class LibraryItemWidget : public QWidget {
Q_OBJECT
public:
    explicit LibraryItemWidget(QWidget *parent = nullptr)
            : QWidget(parent) {
        layout = new QHBoxLayout(this);
        label = new QLabel(this);
        buttonDelete = new QPushButton(this);

        label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

        buttonDelete->setText("Uninstall");

        layout->addWidget(label, 1);
        layout->addWidget(buttonDelete, 0);

        connect(buttonDelete, SIGNAL(pressed()), this, SLOT(onButtonDeletePressed()));

        setLayout(layout);
    }

    void setLibrary(const QString &name) {
        libraryName = name;
        label->setText(name);
    }

    QString getLibrary() {
        return label->text();
    }

signals:

    void onUninstallLibrary(const QString &name);

private:
    QString libraryName;
    QHBoxLayout *layout;
    QLabel *label;
    QPushButton *buttonDelete;

private slots:

    void onButtonDeletePressed() {
        emit onUninstallLibrary(libraryName);
    }
};


#endif //QCALC_LIBRARYITEMWIDGET_HPP
