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


#include "widgets/pythontab.hpp"

#include <QVBoxLayout>
#include <QLine>
#include <QFileDialog>

PythonTab::PythonTab(QWidget *parent)
        : QWidget(parent) {
    modLabel = new QLabel(this);
    modListWidget = new QListWidget(this);
    modDirAddButton = new QPushButton(this);
    modFileAddButton = new QPushButton(this);

    pathLabel = new QLabel(this);
    pathEdit = new QLineEdit(this);

    modListWidget->setToolTip(
            "List of paths that are added to the python sys module path (sys.path) after the interpreter initialized.");

    modLabel->setText("Python Module Paths");
    modFileAddButton->setText("Add File");
    modDirAddButton->setText("Add Directory");

    pathEdit->setPlaceholderText("Override the built-in path... (Restart is required to apply changes)");
    pathEdit->setToolTip(R"(The default path used to initialize python. For separating paths use ";" on windows and ":" on unix.)");

    pathLabel->setText("Python Default Path");
    pathLabel->setToolTip(R"(The default path used to initialize python. For separating paths use ";" on windows and ":" on unix.)");

    auto *hlayout = new QHBoxLayout;
    hlayout->addWidget(modLabel, 1);
    hlayout->addWidget(modFileAddButton, 0);
    hlayout->addWidget(modDirAddButton, 0);

    hlayout->setMargin(0);
    hlayout->setContentsMargins(5, 10, 5, 0);

    auto *pythonModPathContainerWidget = new QWidget(this);
    pythonModPathContainerWidget->setLayout(hlayout);

    pythonModPathContainerWidget->setToolTip(
            "List of paths that are added to the python sys module path (sys.path) after the interpreter initialized.");

    auto *layout = new QVBoxLayout();
    layout->addWidget(pathLabel);
    layout->addWidget(pathEdit);
    layout->addWidget(pythonModPathContainerWidget);
    layout->addWidget(modListWidget, 1);

    setLayout(layout);

    connect(modDirAddButton, SIGNAL(clicked()), this, SLOT(addModDirClick()));
    connect(modFileAddButton, SIGNAL(clicked()), this, SLOT(addModFileClick()));
}

void PythonTab::setPythonModPaths(const std::set<std::string> &paths) {
    modListWidget->clear();
    for (auto &path: paths) {
        addItem(path.c_str());
    }
}

std::set<std::string> PythonTab::getPythonModPaths() {
    std::set<std::string> ret;
    for (auto i = 0; i < modListWidget->count(); i++) {
        auto *item = modListWidget->item(i);
        auto *widget = dynamic_cast<StringEditItemWidget *>(modListWidget->itemWidget(item));
        ret.insert(widget->getText().toStdString());
    }
    return ret;
}

void PythonTab::setPythonPath(const std::string &path) {
    pathEdit->setText(path.c_str());
}

std::string PythonTab::getPythonPath() {
    return pathEdit->text().toStdString();
}

void PythonTab::addModDirClick() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    for (auto &path: fileNames) {
        addItem(path);
    }
}

void PythonTab::addModFileClick() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    for (auto &path: fileNames) {
        addItem(path);
    }
}

void PythonTab::addItem(const QString &path) {
    auto *itemWidget = new StringEditItemWidget(modListWidget);
    itemWidget->setReadOnly(true);
    itemWidget->setDeletable(true);
    itemWidget->setText(path);

    itemWidget->setStyleSheet("QLineEdit{ border: none; }");

    connect(itemWidget, SIGNAL(deleteString(const QString &)), this, SLOT(removeItem(const QString &)));

    auto *item = new QListWidgetItem();
    item->setSizeHint(itemWidget->minimumSizeHint());

    modListWidget->addItem(item);
    modListWidget->setItemWidget(item, itemWidget);
}

void PythonTab::removeItem(const QString &path) {
    for (auto i = 0; i < modListWidget->count(); i++) {
        auto *item = modListWidget->item(i);
        auto *itemWidget = dynamic_cast<StringEditItemWidget *>(modListWidget->itemWidget(item));

        if (itemWidget->getText() == path) {
            modListWidget->removeItemWidget(item);
            modListWidget->takeItem(i);
        }
    }
}
