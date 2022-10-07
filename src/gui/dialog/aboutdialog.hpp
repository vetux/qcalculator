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
        setModal(true);

        setWindowTitle("About " + QApplication::applicationDisplayName());

        text = new QLabel();

        auto t = R"LLL(<p>QCalculator comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it under certain conditions; check <a href="https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt">GPL-2.0</a> for details</p>
<p><a href="https://www.github.com/vetux/qcalculator">Source Code</a></p>
<p>
Powered by:
<a href="https://www.github.com/ArashPartow/exprtk">ExprTk</a>
<a href="https://www.bytereef.org/mpdecimal/">mpdecimal</a>
<a href="https://www.github.com/python/cpython">CPython</a>
<a href="https://www.qt.io">Qt5</a>
</p>
)LLL";
        text->setText("<h1>"
                      + QApplication::applicationDisplayName()
                      + " "
                      + QApplication::applicationVersion()
                      + "</h1><p>Copyright (C) 2022 Julian Zampiccoli</p>"
                      + t);

        text->setTextFormat(Qt::RichText);
        text->setTextInteractionFlags(Qt::TextBrowserInteraction);
        text->setOpenExternalLinks(true);
        text->setWordWrap(true);

        auto *btn = new QPushButton(this);
        btn->setText("Ok");

        auto *btnLayout = new QHBoxLayout;
        btnLayout->addStretch(1);
        btnLayout->addWidget(btn);

        auto *vlayout = new QVBoxLayout();
        vlayout->addWidget(text);
        vlayout->addLayout(btnLayout);

        auto *icon = new QLabel;
        icon->setPixmap(QIcon("qcalculator.ico").pixmap(256));

        auto *hlayout = new QHBoxLayout;
        hlayout->addWidget(icon);
        hlayout->addLayout(vlayout);

        setLayout(hlayout);

        update();

        window()->layout()->setSizeConstraint(QLayout::SetFixedSize);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(accept()));
    }

private:
    QLabel *text;
};

#endif //QCALC_ABOUTDIALOG_HPP
