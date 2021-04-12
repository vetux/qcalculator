#include "gui/widgets/addonitemwidget.hpp"

#include <QPushButton>

#include "gui/addontesterdialog.hpp"

#include "addonmanager.hpp"

AddonItemWidget::AddonItemWidget(QWidget *parent) : QWidget(parent),
                                                    layout(nullptr),
                                                    checkbox(nullptr),
                                                    label(nullptr) {
    layout = new QHBoxLayout(this);
    checkbox = new QCheckBox(this);
    label = new QLabel(this);
    layout->addWidget(checkbox);
    layout->addWidget(label);

    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChange(int)));

#ifdef QT_DEBUG
    button = new QPushButton(this);
    connect(button, SIGNAL(pressed()), this, SLOT(onButtonPressed()));
    layout->addWidget(button);
    button->setText("Run Test");
#endif
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

void AddonItemWidget::onButtonPressed() {
    AddonTesterDialog dialog;
    std::set<std::string> mods = AddonManager::getActiveAddons();
    dialog.setModule(moduleName.toStdString(), mods.find(moduleName.toStdString()) != mods.end());
    dialog.exec();
}
