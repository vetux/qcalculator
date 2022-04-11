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

#ifndef QCALC_ABOUTDIALOG_HPP
#define QCALC_ABOUTDIALOG_HPP

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>

class AboutDialog : public QDialog {
public:
    AboutDialog(QWidget *parent = nullptr)
            : QDialog(parent) {
        setLayout(new QVBoxLayout());

        setModal(true);

        setWindowTitle("About " + QApplication::applicationDisplayName());

        text = new QLabel();

        auto t = R"LLL(<center><p><a href="https://www.github.com/xenotux/qcalc">Source Code</a></p></center>
<p>
Powered by:
<a href="https://www.github.com/ArashPartow/exprtk">ExprTk</a>
<a href="https://www.github.com/python/cpython">CPython</a>
<a href="https://www.qt.io">Qt5</a>
<a href="https://www.mpfr.org">MPFR</a>
</p>
)LLL";
        text->setText("<h1>"
        +QApplication::applicationDisplayName()
                      + " "
                      + QApplication::applicationVersion()
                      + "</h1><p>Copyright (c) 2021 Julian Zampiccoli</p>"
                      + t);

        text->setTextFormat(Qt::RichText);
        text->setTextInteractionFlags(Qt::TextBrowserInteraction);
        text->setOpenExternalLinks(true);

        layout()->addWidget(text);

        update();

        window()->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }

private:
    QLabel *text;
};

#endif //QCALC_ABOUTDIALOG_HPP
