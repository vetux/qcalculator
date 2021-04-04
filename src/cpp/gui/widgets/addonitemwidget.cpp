#include "gui/widgets/addonitemwidget.hpp"

AddonItemWidget::AddonItemWidget(QWidget *parent) : QWidget(parent), layout(nullptr), checkbox(nullptr),
                                                    label(nullptr) {
    layout = new QHBoxLayout(this);
    checkbox = new QCheckBox(this);
    label = new QLabel(this);
    layout->addWidget(checkbox);
    layout->addWidget(label);
    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));
}

void AddonItemWidget::setModuleName(const QString &name) {
    moduleName = name;
}

QString AddonItemWidget::getModuleName() {
    return moduleName;
}

void AddonItemWidget::setModuleEnabled(bool enabled) {
    disconnect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));
    checkbox->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));
}

bool AddonItemWidget::getModuleEnabled() {
    return checkbox->isChecked();
}

void AddonItemWidget::setModuleDisplayName(const QString &name) {
    label->setText(name);
}

void AddonItemWidget::setModuleDescription(const QString &description) {
    label->setToolTip(description);
}

void AddonItemWidget::onCheckBoxStateChange(int state) {
    emit onModuleEnabledChanged(state == Qt::Checked);
}