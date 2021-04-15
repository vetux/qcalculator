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


/**
 * Auto elides the text and exposes a double click signal.
 */
class HistoryLabel : public QLabel {
Q_OBJECT
public:
    explicit HistoryLabel(QWidget *parent = nullptr) : QLabel(parent) {
        // Disable label contents from controlling the minimumSizeHint used by layouts for resizing by setting minimum size.
        setMinimumSize(1, 1);
    }

    void resizeEvent(QResizeEvent *e) override {
        updateText();
        QLabel::resizeEvent(e);
    }

    bool event(QEvent *e) override {
        if (e->type() == QEvent::MouseButtonDblClick) {
            emit onDoubleClick(fullTextData);
            return true;
        }
        return QLabel::event(e);
    }

    QString fullText() {
        return fullTextData;
    }

    void setTextElided(const QString &text) {
        fullTextData = text;
        updateText();
    }

signals:

    void onDoubleClick(const QString &text);

private:
    void updateText() {
        QFontMetrics metrics = QFontMetrics(font());
        QString elitext = metrics.elidedText(fullTextData, Qt::ElideRight, width());
        setText(elitext);
        if (elitext != fullTextData)
            setToolTip(fullTextData);
        else
            setToolTip("");
    }

    QString fullTextData;
};

#endif //QCALC_HISTORYLABEL_HPP
