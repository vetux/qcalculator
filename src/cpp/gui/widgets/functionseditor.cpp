#include "gui/widgets/functionseditor.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "gui/widgets/functionitemwidget.hpp"

FunctionsEditor::FunctionsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());

    list = new QListWidget(this);

    addLineEdit = new QLineEdit(this);
    addPushButton = new QPushButton(this);

    argsSpinBox = new QSpinBox(this);
    argEdit0 = new QLineEdit(this);
    argEdit1 = new QLineEdit(this);
    argEdit2 = new QLineEdit(this);
    argEdit3 = new QLineEdit(this);
    argEdit4 = new QLineEdit(this);

    expressionEdit = new QTextEdit(this);

    auto *widgetAdd = new QWidget(this);
    widgetAdd->setLayout(new QHBoxLayout());
    widgetAdd->layout()->addWidget(addLineEdit);
    widgetAdd->layout()->addWidget(addPushButton);

    auto *widgetArgs = new QWidget(this);
    widgetArgs->setLayout(new QHBoxLayout());
    widgetArgs->layout()->addWidget(argsSpinBox);
    widgetArgs->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding));
    widgetArgs->layout()->addWidget(argEdit0);
    widgetArgs->layout()->addWidget(argEdit1);
    widgetArgs->layout()->addWidget(argEdit2);
    widgetArgs->layout()->addWidget(argEdit3);
    widgetArgs->layout()->addWidget(argEdit4);

    auto *widgetLeft = new QWidget(this);
    widgetLeft->setLayout(new QVBoxLayout());
    widgetLeft->layout()->addWidget(widgetAdd);
    widgetLeft->layout()->addWidget(list);

    auto *widgetTop = new QWidget(this);
    widgetTop->setLayout(new QHBoxLayout());
    widgetTop->layout()->addWidget(widgetLeft);
    widgetTop->layout()->addWidget(expressionEdit);

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
    connect(list, SIGNAL(currentItemChanged(QListWidgetItem * , QListWidgetItem * )), this,
            SLOT(onListWidgetItemChanged(QListWidgetItem * , QListWidgetItem * )));
    connect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));
}

void FunctionsEditor::setFunctions(const std::map<std::string, Function> &functions) {
    disconnect(list, SIGNAL(currentItemChanged(QListWidgetItem * , QListWidgetItem * )), this,
               SLOT(onListWidgetItemChanged(QListWidgetItem * , QListWidgetItem * )));
    disconnect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));
    disconnect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));

    this->functions = functions;
    addLineEdit->setText("");

    applyArgs({});

    expressionEdit->setText("");
    expressionEdit->setEnabled(false);

    argsSpinBox->setEnabled(false);
    argsSpinBox->setValue(0);

    widgets.clear();

    list->clear();
    for (auto &p : functions) {
        auto *widget = new FunctionItemWidget(list);
        auto *item = new QListWidgetItem(list);

        widget->setName(p.first.c_str());

        item->setSizeHint(widget->sizeHint());

        connect(widget, SIGNAL(editingFinished()), this, SLOT(onFunctionNameEditingFinished()));

        list->addItem(item);
        list->setItemWidget(item, widget);

        widgets[p.first] = item;
    }

    connect(list, SIGNAL(currentItemChanged(QListWidgetItem * , QListWidgetItem * )), this,
            SLOT(onListWidgetItemChanged(QListWidgetItem * , QListWidgetItem * )));
    connect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));
    connect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));
}

void FunctionsEditor::setCurrentFunction(const QString &name) {
    currentFunction = name.toStdString();
    if (widgets.find(currentFunction) != widgets.end())
        list->setCurrentItem(widgets.at(currentFunction));
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

void FunctionsEditor::onListWidgetItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if (current == nullptr) {
        disconnect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));
        expressionEdit->setEnabled(false);
        expressionEdit->setText("");
        connect(expressionEdit, SIGNAL(textChanged()), this, SLOT(onFunctionExpressionChanged()));

        disconnect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));
        argsSpinBox->setEnabled(false);
        argsSpinBox->setValue(0);
        connect(argsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFunctionArgsSpinBoxChanged(int)));

        applyArgs({});

        emit onCurrentFunctionChanged("");
    } else if (current != previous) {
        auto &widget = dynamic_cast<FunctionItemWidget&>(*list->itemWidget(current));
        currentFunction = widget.getName().toStdString();
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
}

void FunctionsEditor::onFunctionNameEditingFinished() {
    auto &edit = dynamic_cast<FunctionItemWidget &>(*sender());
    if (!edit.isModified())
        return;
    if (edit.getName() != edit.text()) {
        edit.setModified(false);
        emit onFunctionNameChanged(edit.getName(), edit.text());
    }
}

void FunctionsEditor::onFunctionExpressionChanged() {
    if (!currentFunction.empty()) {
        auto cursor = expressionEdit->textCursor();
        emit onFunctionBodyChanged(currentFunction.c_str(), expressionEdit->toPlainText());
        expressionEdit->setTextCursor(cursor);
        expressionEdit->setFocus();
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