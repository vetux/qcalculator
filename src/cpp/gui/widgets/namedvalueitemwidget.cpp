#include "gui/widgets/namedvalueitemwidget.hpp"

#include <QHBoxLayout>

NamedValueItemWidget::NamedValueItemWidget(QWidget *parent) : QWidget(parent) {
    setLayout(new QHBoxLayout(this));

    nameEdit = new QLineEdit(this);
    valueEdit = new QLineEdit(this);

    connect(nameEdit, SIGNAL(editingFinished()), this, SLOT(onNameEditingFinished()));
    connect(valueEdit, SIGNAL(editingFinished()), this, SLOT(onValueEditingFinished()));

    layout()->addWidget(nameEdit);
    layout()->addWidget(valueEdit);
}

QString NamedValueItemWidget::getName() {
    return nameText;
}

QString NamedValueItemWidget::getValue() {
    return valueText;
}

void NamedValueItemWidget::setName(const QString &name) {
    nameText = name;
    nameEdit->setText(nameText);
}

void NamedValueItemWidget::setValue(const QString &value) {
    valueText = value;
    valueEdit->setText(valueText);
}

void NamedValueItemWidget::onNameEditingFinished() {
    if (!nameEdit->isModified())
        return;

    QString content = nameEdit->text();
    if (nameText != content) {
        nameEdit->setModified(false);
        emit onNameChanged(content);
    }
}

void NamedValueItemWidget::onValueEditingFinished() {
    if (!valueEdit->isModified())
        return;

    QString content = valueEdit->text();
    if (valueText != content) {
        valueEdit->setModified(false);
        emit onValueChanged(content);
    }
}
