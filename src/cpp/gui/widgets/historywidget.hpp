#ifndef QCALC_HISTORYWIDGET_HPP
#define QCALC_HISTORYWIDGET_HPP

#include <QListWidget>

#include "gui/widgets/historyitemwidget.hpp"

//TODO: Fix history overlapping scrollbar when reducing window size with different tab active.
class HistoryWidget : public QListWidget {
Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setSelectionMode(QAbstractItemView::NoSelection);
        setVerticalScrollMode(ScrollPerPixel);
        setFocusPolicy(Qt::NoFocus);
    }

public slots:

    void setContent(const QList<QPair<QString, QString>> &c) {
        clear();
        for (auto &pair : c) {
            addContent(pair.first, pair.second);
        }
    }

    void addContent(const QString &expression, const QString &value) {
        auto *widget = new HistoryItemWidget(this);
        auto *item = new QListWidgetItem(this);

        widget->setContents(expression, value);

        item->setSizeHint(widget->sizeHint());

        addItem(item);
        setItemWidget(item, widget);

        update();

        scrollToBottom();
    }
};

#endif //QCALC_HISTORYWIDGET_HPP
