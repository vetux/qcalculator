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

#include <QEvent>
#include <QLineEdit>
#include <QKeyEvent>

/**
 * Auto elides the text and exposes a double click signal.
 */
class HistoryLabel : public QLineEdit {
Q_OBJECT
public:
    explicit HistoryLabel(QWidget *parent = nullptr) : QLineEdit(parent) {
        setStyleSheet("QLineEdit { border: none; background: transparent; }");
    }

    bool event(QEvent *e) override {
        if (e->type() == QEvent::MouseButtonDblClick) {
            emit onDoubleClick(text());
            return true;
        }
        return QLineEdit::event(e);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Left
            || event->key() == Qt::Key_Right
            || event->key() == Qt::Key_Home
            || event->key() == Qt::Key_End
            || (event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier)
            || (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier)
            || event->key() == Qt::Key_Insert) {
            QLineEdit::keyPressEvent(event);
        } else {
            event->ignore();
        }
    }

signals:

    void onDoubleClick(const QString &text);

};

#endif //QCALC_HISTORYLABEL_HPP
