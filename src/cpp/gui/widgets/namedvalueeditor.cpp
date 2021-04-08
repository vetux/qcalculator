#include "gui/widgets/namedvalueeditor.hpp"

#include <QListWidget>
#include <QVBoxLayout>

#include "gui/widgets/namedvalueitemwidget.hpp"

NamedValueEditor::NamedValueEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());
    list = new QListWidget(this);

    addLineEditName = new QLineEdit(this);
    addLineEditValue = new QLineEdit(this);
    addPushButton = new QPushButton(this);

    addPushButton->setText("Add");

    connect(addPushButton, SIGNAL(pressed()), this, SLOT(onAddPressed()));

    auto *widgetAdd = new QWidget(this);
    widgetAdd->setLayout(new QHBoxLayout(widgetAdd));
    widgetAdd->layout()->addWidget(addLineEditName);
    widgetAdd->layout()->addWidget(addLineEditValue);
    widgetAdd->layout()->addWidget(addPushButton);

    layout()->addWidget(widgetAdd);
    layout()->addWidget(list);
}

void NamedValueEditor::setValues(const std::map<QString, QString> &values) {
    addLineEditName->setText("");
    addLineEditValue->setText("");

    list->clear();

    for (auto &p : values) {
        auto *widget = new NamedValueItemWidget(list);
        auto *item = new QListWidgetItem(list);

        widget->setName(p.first);
        widget->setValue(p.second);

        item->setSizeHint(widget->sizeHint());

        connect(widget, SIGNAL(onNameChanged(const QString &)), this, SLOT(onNameChanged(const QString &)));
        connect(widget, SIGNAL(onValueChanged(const QString &)), this, SLOT(onValueChanged(const QString &)));

        list->addItem(item);
        list->setItemWidget(item, widget);

        list->update();
    }
}

void NamedValueEditor::onAddPressed() {
    emit onNamedValueAdded(addLineEditName->text(), addLineEditValue->text());
}

void NamedValueEditor::onNameChanged(const QString &name) {
    auto &widget = dynamic_cast<NamedValueItemWidget &>(*sender());

    QString originalName = widget.getName();

    if (name != originalName)
            emit onNameChanged(originalName, name);
}

void NamedValueEditor::onValueChanged(const QString &value) {
    auto &widget = dynamic_cast<NamedValueItemWidget &>(*sender());

    QString originalName = widget.getName();
    QString originalValue = widget.getValue();

    if (value != originalValue)
            emit onValueChanged(originalName, value);
}