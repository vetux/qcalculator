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

#ifndef QCALC_HISTORYLABEL_HPP
#define QCALC_HISTORYLABEL_HPP

#include <QLabel>
#include <QEvent>

class HistoryLabel : public QLabel {
Q_OBJECT
public:
    explicit HistoryLabel(QWidget *parent = nullptr) : QLabel(parent) {
    }

    bool event(QEvent *e) override {
        if (e->type() == QEvent::MouseButtonDblClick) {
            emit onDoubleClick(text());
            return true;
        }
        return QLabel::event(e);
    }

    QString fullText() {
        return ft;
    }
    
    void setTextElided(const QString &text) {
        ft = text;
        QFontMetrics metrics = QFontMetrics(font());
        QString elitext = metrics.elidedText(text, Qt::ElideRight, width());
        setText(elitext);
        if (elitext != text)
            setToolTip(text);
        else
            setToolTip("");
    }

signals:

    void onDoubleClick(const QString &text);

private:
    QString ft;
};

#endif //QCALC_HISTORYLABEL_HPP
