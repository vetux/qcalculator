#include "gui/widgets/functionseditor.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>

FunctionsEditor::FunctionsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());

    list = new QTableWidget(this);

    list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    addLineEdit = new QLineEdit(this);
    addPushButton = new QPushButton(this);

    argsSpinBox = new QSpinBox(this);
    argEdit0 = new QLineEdit(this);
    argEdit1 = new QLineEdit(this);
    argEdit2 = new QLineEdit(this);
    argEdit3 = new QLineEdit(this);
    argEdit4 = new QLineEdit(this);

    expressionEdit = new QTextEdit(this);

    list->horizontalHeader()->hide();
    list->verticalHeader()->hide();

    list->horizontalHeader()->setStretchLastSection(true);

    auto *widgetAdd = new QWidget(this);
    widgetAdd->setLayout(new QHBoxLayout());
    widgetAdd->layout()->addWidget(addLineEdit);
    widgetAdd->layout()->addWidget(addPushButton);
    widgetAdd->layout()->setContentsMargins(0, 0, 0, 0);

    auto *widgetArgs = new QWidget(this);
    widgetArgs->setLayout(new QHBoxLayout());
    widgetArgs->layout()->addWidget(argsSpinBox);
    widgetArgs->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding));
    widgetArgs->layout()->addWidget(argEdit0);
    widgetArgs->layout()->addWidget(argEdit1);
    widgetArgs->layout()->addWidget(argEdit2);
    widgetArgs->layout()->addWidget(argEdit3);
    widgetArgs->layout()->addWidget(argEdit4);
    widgetArgs->layout()->setContentsMargins(0, 0, 0, 0);

    auto *widgetLeft = new QWidget(this);
    widgetLeft->setLayout(new QVBoxLayout());
    widgetLeft->layout()->addWidget(widgetAdd);
    widgetLeft->layout()->addWidget(list);
    widgetLeft->layout()->setContentsMargins(0, 0, 0, 0);
    widgetLeft->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto *widgetTop = new QWidget(this);
    widgetTop->setLayout(new QHBoxLayout());
    widgetTop->layout()->addWidget(widgetLeft);
    widgetTop->layout()->addWidget(expressionEdit);
    widgetTop->layout()->setContentsMargins(0, 0, 0, 0);

    layout()->addWidget(widgetTop);
    layout()->addWidget(widgetArgs);

    addPushButton->setText("Add");

    argsSpinBox->setMaximum(5);

    connect(addPushButton, SIGNAL(pressed()), this, SLOT(onFunctionAddPressed()));

    connect(argEdit0, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit1, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit2, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit3, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit4, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));

    connect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));
    connect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));

    connect(list, SIGNAL(cellClicked(int, int)), this, SLOT(onTableCellActivated(int, int)));
    connect(list, SIGNAL(cellChanged(int, int)), this, SLOT(onTableCellChanged(int, int)));
}

void FunctionsEditor::setFunctions(const std::map<std::string, Function> &f) {
    disconnect(list, SIGNAL(cellClicked(int, int)), this, SLOT(onTableCellActivated(int, int)));
    disconnect(list, SIGNAL(cellChanged(int, int)), this, SLOT(onTableCellChanged(int, int)));

    disconnect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));
    disconnect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));

    disconnect(argEdit0, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    disconnect(argEdit1, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    disconnect(argEdit2, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    disconnect(argEdit3, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    disconnect(argEdit4, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));

    this->functions = f;
    addLineEdit->setText("");

    applyArgs({});

    expressionEdit->setText("");
    expressionEdit->setEnabled(false);

    argsSpinBox->setEnabled(false);
    argsSpinBox->setValue(0);

    rowMapping.clear();
    list->clear();
    list->setColumnCount(1);
    list->setRowCount(functions.size());
    int i = 0;
    for (auto &p : functions) {
        rowMapping[p.first] = i;
        auto *item = new QTableWidgetItem(p.first.c_str());
        list->setItem(i++, 0, item);
    }

    connect(list, SIGNAL(cellClicked(int, int)), this, SLOT(onTableCellActivated(int, int)));
    connect(list, SIGNAL(cellChanged(int, int)), this, SLOT(onTableCellChanged(int, int)));

    connect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));
    connect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));

    connect(argEdit0, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit1, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit2, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit3, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
    connect(argEdit4, SIGNAL(editingFinished()), this, SLOT(onFunctionArgEditingFinished()));
}

void FunctionsEditor::setCurrentFunction(const QString &name) {
    currentFunction = name.toStdString();
    if (rowMapping.find(currentFunction) != rowMapping.end()) {
        list->setCurrentCell(rowMapping.at(currentFunction), 0);
        list->cellClicked(rowMapping.at(currentFunction), 0);
    }
}

void FunctionsEditor::onFunctionAddPressed() {
    emit onFunctionAdded(addLineEdit->text());
}

void FunctionsEditor::onFunctionArgEditingFinished() {
    auto &edit = dynamic_cast<QLineEdit &>(*sender());
    std::vector<std::string> currentArgs = functions.at(currentFunction).argumentNames;
    if (&edit == argEdit0) {
        currentArgs.at(0) = edit.text().toStdString();
    } else if (&edit == argEdit1) {
        currentArgs.at(1) = edit.text().toStdString();
    } else if (&edit == argEdit2) {
        currentArgs.at(2) = edit.text().toStdString();
    } else if (&edit == argEdit3) {
        currentArgs.at(3) = edit.text().toStdString();
    } else if (&edit == argEdit4) {
        currentArgs.at(4) = edit.text().toStdString();
    }
    emit onFunctionArgsChanged(currentFunction.c_str(), currentArgs);
}

void FunctionsEditor::onFunctionArgsSpinBoxChanged(int value) {
    Function func = functions.at(currentFunction);
    func.argumentNames.resize(value);
    emit onFunctionArgsChanged(currentFunction.c_str(), func.argumentNames);
}

void FunctionsEditor::onTableCellActivated(int row, int column) {
    currentFunction = list->item(row, column)->text().toStdString();

    Function func = functions.at(currentFunction);

    disconnect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));
    expressionEdit->setEnabled(true);
    expressionEdit->setText(func.expression.c_str());
    connect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));

    disconnect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));
    argsSpinBox->setEnabled(true);
    argsSpinBox->setValue(func.argumentNames.size());
    connect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));

    applyArgs(func.argumentNames);

    emit onCurrentFunctionChanged(currentFunction.c_str());
}

void FunctionsEditor::onTableCellChanged(int row, int column) {
    std::string originalName;
    for (auto &p : rowMapping) {
        if (p.second == row) {
            originalName = p.first;
            break;
        }
    }

    emit onFunctionNameChanged(originalName.c_str(), list->item(row, column)->text());
}

void FunctionsEditor::onFunctionExpressionChanged() {
    if (!currentFunction.empty()) {
        // QTextCursor seems to have an internal reference to the text edit therefore we take the position
        // and call setPosition on the cursor object returned after setting the text.
        int cursorPos = expressionEdit->textCursor().position();
        auto scrollPos = expressionEdit->verticalScrollBar()->sliderPosition();
        emit onFunctionBodyChanged(currentFunction.c_str(), expressionEdit->toPlainText());
        expressionEdit->setFocus();
        auto cursor = expressionEdit->textCursor();
        cursor.setPosition(cursorPos);
        expressionEdit->setTextCursor(cursor);
        expressionEdit->verticalScrollBar()->setSliderPosition(scrollPos);
    }
}

void FunctionsEditor::applyArgs(const std::vector<std::string> &args) {
    argEdit4->setVisible(false);
    argEdit4->setText("");
    argEdit3->setVisible(false);
    argEdit3->setText("");
    argEdit2->setVisible(false);
    argEdit2->setText("");
    argEdit1->setVisible(false);
    argEdit1->setText("");
    argEdit0->setVisible(false);
    argEdit0->setText("");

    switch (args.size()) {
        case 5:
            argEdit4->setVisible(true);
            argEdit4->setText(args.at(4).c_str());
        case 4:
            argEdit3->setVisible(true);
            argEdit3->setText(args.at(3).c_str());
        case 3:
            argEdit2->setVisible(true);
            argEdit2->setText(args.at(2).c_str());
        case 2:
            argEdit1->setVisible(true);
            argEdit1->setText(args.at(1).c_str());
        case 1:
            argEdit0->setVisible(true);
            argEdit0->setText(args.at(0).c_str());
            break;
    }
}