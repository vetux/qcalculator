#ifndef QCALC_HISTORYWIDGET_HPP
#define QCALC_HISTORYWIDGET_HPP

#include <QListWidget>

#include "gui/widgets/historyitemwidget.hpp"

class HistoryWidget : public QListWidget {
Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr) : QListWidget(parent) {}

public slots:

    void setContent(const QList<QPair<QString, QString>> &c) {
        clear();
        for (auto &pair : c) {
            addContent(pair);
        }
    }

    void addContent(const QPair<QString, QString> &c) {
        auto *widget = new HistoryItemWidget(this);
        auto *item = new QListWidgetItem(this);

        widget->setContents(c.first, c.second);

        item->setSizeHint(widget->sizeHint());

        addItem(item);
        setItemWidget(item, widget);

        update();
    }
};

#endif //QCALC_HISTORYWIDGET_HPP
