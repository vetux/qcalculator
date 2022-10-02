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


#include "pythontab.hpp"

#include <QVBoxLayout>
#include <QLine>
#include <QFileDialog>

PythonTab::PythonTab(QWidget *parent)
        : QWidget(parent) {
    pythonModPathLabel = new QLabel(this);
    pythonModPathListWidget = new QListWidget(this);
    pythonModPathAddPushButton = new QPushButton(this);

    pythonModPathListWidget->setToolTip("List of paths that are added to the python module search path");

    pythonModPathLabel->setText("Python Module Paths");
    pythonModPathAddPushButton->setText("Add Directory");

    pythonPathLabel = new QLabel(this);
    pythonPathEdit = new QLineEdit(this);

    pythonPathEdit->setPlaceholderText("Override default path...");

    pythonPathLabel->setText("Python Path");

    auto *hlayout = new QHBoxLayout;
    hlayout->addWidget(pythonModPathLabel, 1);
    hlayout->addWidget(pythonModPathAddPushButton, 0);

    hlayout->setMargin(0);
    hlayout->setContentsMargins(5, 10, 5, 0);

    auto *pythonModPathContainerWidget = new QWidget(this);
    pythonModPathContainerWidget->setLayout(hlayout);

    pythonModPathContainerWidget->setToolTip(
            "List of paths that are added to the python module search path");

    auto *layout = new QVBoxLayout();
    layout->addWidget(pythonPathLabel);
    layout->addWidget(pythonPathEdit);
    layout->addWidget(pythonModPathContainerWidget);
    layout->addWidget(pythonModPathListWidget, 1);

    setLayout(layout);

    connect(pythonModPathAddPushButton, SIGNAL(clicked()), this, SLOT(addModPathPressed()));
}

void PythonTab::setPythonModPaths(const std::set<std::string> &paths) {
    pythonModPathListWidget->clear();
    for (auto &path: paths) {
        addItem(path.c_str());
    }
}

std::set<std::string> PythonTab::getPythonModPaths() {
    std::set<std::string> ret;
    for (auto i = 0; i < pythonModPathListWidget->count(); i++) {
        auto *item = pythonModPathListWidget->item(i);
        auto *widget = dynamic_cast<StringEditItemWidget *>(pythonModPathListWidget->itemWidget(item));
        ret.insert(widget->getText().toStdString());
    }
    return ret;
}

void PythonTab::setPythonPath(const std::string &path) {
    pythonPathEdit->setText(path.c_str());
}

std::string PythonTab::getPythonPath() {
    return pythonPathEdit->text().toStdString();
}

void PythonTab::addModPathPressed() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    for (auto &path: fileNames) {
        addItem(path);
    }
}

void PythonTab::addItem(const QString &path) {
    auto *itemWidget = new StringEditItemWidget(pythonModPathListWidget);
    itemWidget->setReadOnly(true);
    itemWidget->setDeletable(true);
    itemWidget->setText(path);

    connect(itemWidget, SIGNAL(deleteString(const QString &)), this, SLOT(removeItem(const QString &)));

    auto *item = new QListWidgetItem();
    item->setSizeHint(itemWidget->minimumSizeHint());

    pythonModPathListWidget->addItem(item);
    pythonModPathListWidget->setItemWidget(item, itemWidget);
}

void PythonTab::removeItem(const QString &path) {
    for (auto i = 0; i < pythonModPathListWidget->count(); i++) {
        auto *item = pythonModPathListWidget->item(i);
        auto *itemWidget = dynamic_cast<StringEditItemWidget *>(pythonModPathListWidget->itemWidget(item));

        if (itemWidget->getText() == path) {
            pythonModPathListWidget->removeItemWidget(item);
            pythonModPathListWidget->takeItem(i);
        }
    }
}
