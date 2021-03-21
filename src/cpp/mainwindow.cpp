#include "mainwindow.hpp"

#include <QScrollBar>
#include <QMessageBox>
#include <QTextStream>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSizePolicy>

#include "ui_mainwindow.h"

#include "numberformat.hpp"
#include "fractiontest.hpp"

#define OBJECT_NAME_PREFIX_BITS "pushButton_bit_"
#define OBJECT_NAME_PREFIX_KEYPAD "pushButton_kp_"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          ui(new Ui::MainWindow),
          model(),
          presenter(model, *this) {
    ui->setupUi(this);

    connect(ui->lineEdit_input_decimal, SIGNAL(editingFinished()), this, SLOT(onNumericSystemsFinishEditing()));
    connect(ui->lineEdit_input_hex, SIGNAL(editingFinished()), this, SLOT(onNumericSystemsFinishEditing()));
    connect(ui->lineEdit_input_octal, SIGNAL(editingFinished()), this, SLOT(onNumericSystemsFinishEditing()));
    connect(ui->lineEdit_input_binary, SIGNAL(editingFinished()), this, SLOT(onNumericSystemsFinishEditing()));

    for (int i = NumPadKey::START + 1; i < NumPadKey::END; i++) {
        connect(&getKeyPadButton(static_cast<NumPadKey>(i)), SIGNAL(pressed()), this, SLOT(onNumPadKeyPressed()));
    }

    for (int i = 0; i < 64; i++) {
        connect(&getBitButton(i), SIGNAL(pressed()), this, SLOT(onBitViewKeyPressed()));
    }

    connect(ui->tableWidget_variables, SIGNAL(itemSelectionChanged()), this, SLOT(onVariablesSelectionChanged()));
    connect(ui->tableWidget_variables, SIGNAL(cellChanged(int, int)), this, SLOT(onVariablesCellChanged(int, int)));

    connect(ui->tableWidget_constants, SIGNAL(itemSelectionChanged()), this, SLOT(onConstantsSelectionChanged()));
    connect(ui->tableWidget_constants, SIGNAL(cellChanged(int, int)), this, SLOT(onConstantsCellChanged(int, int)));

    connect(ui->tableWidget_functions, SIGNAL(itemSelectionChanged()), this, SLOT(onFunctionsSelectionChanged()));
    connect(ui->tableWidget_functions, SIGNAL(cellChanged(int, int)), this, SLOT(onFunctionsCellChanged(int, int)));
    connect(ui->spinBox_functions_argcount, SIGNAL(valueChanged(int)), this,
            SLOT(onFunctionsArgCountValueChanged(int)));
    connect(ui->lineEdit_functions_arg0, SIGNAL(editingFinished()), this, SLOT(onFunctionsArgEditingFinished()));
    connect(ui->lineEdit_functions_arg1, SIGNAL(editingFinished()), this, SLOT(onFunctionsArgEditingFinished()));
    connect(ui->lineEdit_functions_arg2, SIGNAL(editingFinished()), this, SLOT(onFunctionsArgEditingFinished()));
    connect(ui->lineEdit_functions_arg3, SIGNAL(editingFinished()), this, SLOT(onFunctionsArgEditingFinished()));
    connect(ui->lineEdit_functions_arg4, SIGNAL(editingFinished()), this, SLOT(onFunctionsArgEditingFinished()));
    connect(ui->textEdit_functions_body, SIGNAL(textChanged()), this, SLOT(onFunctionsBodyTextChanged()));

    connect(ui->tableWidget_scripts, SIGNAL(itemSelectionChanged()), this, SLOT(onScriptsSelectionChanged()));
    connect(ui->tableWidget_scripts, SIGNAL(cellChanged(int, int)), this, SLOT(onScriptsCellChanged(int, int)));
    connect(ui->textEdit_scripts, SIGNAL(textChanged()), this, SLOT(onScriptsBodyTextChanged()));

    model.setStateListener(presenter);

    presenter.init();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    presenter.onWindowClose(*event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    presenter.onWindowResize(*event);
}

//+View
void MainWindow::connectPresenter(const Presenter &target) {
    connect(ui->lineEdit_input, SIGNAL(returnPressed()), &target, SLOT(onInputSubmit()));
    connect(ui->lineEdit_input, SIGNAL(textChanged(const QString &)), &target, SLOT(onInputUpdate(const QString &)));

    connect(this, SIGNAL(signalDecimalSubmit(QString)), &target, SLOT(onDecimalSubmit(QString)));
    connect(this, SIGNAL(signalHexSubmit(QString)), &target, SLOT(onHexSubmit(QString)));
    connect(this, SIGNAL(signalOctalSubmit(QString)), &target, SLOT(onOctalSubmit(QString)));
    connect(this, SIGNAL(signalBinarySubmit(QString)), &target, SLOT(onBinarySubmit(QString)));

    connect(this, SIGNAL(signalNumPadKeyPressed(NumPadKey)), &target, SLOT(onNumPadKeyPressed(NumPadKey)));
    connect(this, SIGNAL(signalBitViewKeyPressed(int)), &target, SLOT(onBitViewKeyPressed(int)));

    connect(this, SIGNAL(signalSelectedVariableChanged(int)), &target, SLOT(onSelectedVariableChanged(int)));
    connect(this, SIGNAL(signalVariableChanged(std::string, std::string)), &target,
            SLOT(onVariableChanged(std::string, std::string)));

    connect(this, SIGNAL(signalSelectedConstantChanged(int)), &target, SLOT(onSelectedConstantChanged(int)));
    connect(this, SIGNAL(signalConstantChanged(std::string, std::string)), &target,
            SLOT(onConstantChanged(std::string, std::string)));

    connect(this, SIGNAL(signalSelectedFunctionChanged(int)), &target, SLOT(onSelectedFunctionChanged(int)));
    connect(this, SIGNAL(signalFunctionNameChanged(std::string)), &target, SLOT(onFunctionNameChanged(std::string)));
    connect(this, SIGNAL(signalFunctionBodyChanged(std::string)), &target, SLOT(onFunctionBodyChanged(std::string)));
    connect(this, SIGNAL(signalFunctionArgsChanged(std::vector<std::string>)), &target,
            SLOT(onFunctionArgsChanged(std::vector<std::string>)));

    connect(this, SIGNAL(signalSelectedScriptChanged(int)), &target, SLOT(onSelectedScriptChanged(int)));
    connect(this, SIGNAL(signalScriptNameChanged(std::string)), &target, SLOT(onScriptNameChanged(std::string)));
    connect(this, SIGNAL(signalScriptBodyChanged(std::string)), &target, SLOT(onScriptBodyChanged(std::string)));
    connect(this, SIGNAL(signalScriptBodyChanged(std::string)), &target, SLOT(onScriptBodyChanged(std::string)));
    connect(ui->checkBox_scripts_enableargs, SIGNAL(toggled(bool)), &target, SLOT(onScriptEnableArgsChanged(bool)));

    connect(ui->actionExit, SIGNAL(triggered(bool)), &target, SLOT(onActionExit()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), &target, SLOT(onActionAbout()));
    connect(ui->actionShow_Keypad, SIGNAL(toggled(bool)), &target, SLOT(onActionShowKeyPad(bool)));
    connect(ui->actionShow_Bit_Toggle, SIGNAL(toggled(bool)), &target, SLOT(onActionShowBitView(bool)));
    connect(ui->actionShow_Dock, SIGNAL(toggled(bool)), &target, SLOT(onActionShowDock(bool)));
    connect(ui->actionImport_Symbols, SIGNAL(triggered(bool)), &target, SLOT(onActionImportSymbolTable()));
    connect(ui->actionExport_Symbols, SIGNAL(triggered(bool)), &target, SLOT(onActionExportSymbolTable()));

    connect(ui->tabWidget_2, SIGNAL(currentChanged(int)), &target, SLOT(onDockTabChanged(int)));
    connect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), &target, SLOT(onDockVisibilityChanged(bool)));
    connect(ui->dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), &target, SLOT(onDockPositionChanged(Qt::DockWidgetArea)));
}

void MainWindow::disconnectPresenter(const Presenter &target) {
    disconnect(ui->lineEdit_input, SIGNAL(returnPressed()), &target, SLOT(onInputSubmit()));
    disconnect(ui->lineEdit_input, SIGNAL(textChanged(const QString &)), &target, SLOT(onInputUpdate(const QString &)));

    disconnect(this, SIGNAL(signalDecimalSubmit(QString)), &target, SLOT(onDecimalSubmit(QString)));
    disconnect(this, SIGNAL(signalHexSubmit(QString)), &target, SLOT(onHexSubmit(QString)));
    disconnect(this, SIGNAL(signalOctalSubmit(QString)), &target, SLOT(onOctalSubmit(QString)));
    disconnect(this, SIGNAL(signalBinarySubmit(QString)), &target, SLOT(onBinarySubmit(QString)));

    disconnect(this, SIGNAL(signalNumPadKeyPressed(NumPadKey)), &target, SLOT(onNumPadKeyPressed(NumPadKey)));
    disconnect(this, SIGNAL(signalBitViewKeyPressed(int)), &target, SLOT(onBitViewKeyPressed(int)));

    disconnect(this, SIGNAL(signalSelectedVariableChanged(int)), &target, SLOT(onSelectedVariableChanged(int)));
    disconnect(this, SIGNAL(signalVariableChanged(std::string, std::string)), &target,
               SLOT(onVariableChanged(std::string, std::string)));

    disconnect(this, SIGNAL(signalSelectedConstantChanged(int)), &target, SLOT(onSelectedConstantChanged(int)));
    disconnect(this, SIGNAL(signalConstantChanged(std::string, std::string)), &target,
               SLOT(onConstantChanged(std::string, std::string)));

    disconnect(this, SIGNAL(signalSelectedFunctionChanged(int)), &target, SLOT(onSelectedFunctionChanged(int)));
    disconnect(this, SIGNAL(signalFunctionNameChanged(std::string)), &target, SLOT(onFunctionNameChanged(std::string)));
    disconnect(this, SIGNAL(signalFunctionBodyChanged(std::string)), &target, SLOT(onFunctionBodyChanged(std::string)));
    disconnect(this, SIGNAL(signalFunctionArgsChanged(std::vector<std::string>)), &target,
               SLOT(onFunctionArgsChanged(std::vector<std::string>)));

    disconnect(this, SIGNAL(signalSelectedScriptChanged(int)), &target, SLOT(onSelectedScriptChanged(int)));
    disconnect(this, SIGNAL(signalScriptNameChanged(std::string)), &target, SLOT(onScriptNameChanged(std::string)));
    disconnect(this, SIGNAL(signalScriptBodyChanged(std::string)), &target, SLOT(onScriptBodyChanged(std::string)));
    disconnect(this, SIGNAL(signalScriptBodyChanged(std::string)), &target, SLOT(onScriptBodyChanged(std::string)));
    disconnect(ui->checkBox_scripts_enableargs, SIGNAL(toggled(bool)), &target, SLOT(onScriptEnableArgsChanged(bool)));

    disconnect(ui->actionExit, SIGNAL(triggered(bool)), &target, SLOT(onActionExit()));
    disconnect(ui->actionAbout, SIGNAL(triggered(bool)), &target, SLOT(onActionAbout()));
    disconnect(ui->actionShow_Keypad, SIGNAL(toggled(bool)), &target, SLOT(onActionShowKeyPad(bool)));
    disconnect(ui->actionShow_Bit_Toggle, SIGNAL(toggled(bool)), &target, SLOT(onActionShowBitView(bool)));
    disconnect(ui->actionShow_Dock, SIGNAL(toggled(bool)), &target, SLOT(onActionShowDock(bool)));
    disconnect(ui->actionImport_Symbols, SIGNAL(triggered(bool)), &target, SLOT(onActionImportSymbolTable()));
    disconnect(ui->actionExport_Symbols, SIGNAL(triggered(bool)), &target, SLOT(onActionExportSymbolTable()));

    disconnect(ui->tabWidget_2, SIGNAL(currentChanged(int)), &target, SLOT(onDockTabChanged(int)));
    disconnect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), &target, SLOT(onDockVisibilityChanged(bool)));
    disconnect(ui->dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), &target, SLOT(onDockPositionChanged(Qt::DockWidgetArea)));
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, "About " + QApplication::applicationDisplayName(),
                       QApplication::applicationDisplayName()
                       + " "
                       + QApplication::applicationVersion()
                       + "\n\nCopyright (c) 2021 Julian Zampiccoli"
                       + "\n\n"
                       + QString(R"LLL(Source:
    https://github.com/xenotux/qcalc

Powered by:
    https://github.com/ArashPartow/exprtk
    https://github.com/qt
)LLL"));
}

void MainWindow::showSettingsDialog() {

}

void MainWindow::showWarningDialog(const std::string &title, const std::string &text) {
    QMessageBox::warning(this, title.c_str(), text.c_str());
}

void MainWindow::showInfoDialog(const std::string &title, const std::string &text) {
    QMessageBox::information(this, title.c_str(), text.c_str());
}

bool MainWindow::showFileChooserDialog(const std::string &title, bool existingFile, std::string &filePathOut) {
    QFileDialog dialog(this);
    if (existingFile)
        dialog.setFileMode(QFileDialog::ExistingFile);
    else
        dialog.setFileMode(QFileDialog::AnyFile);

    if (dialog.exec()) {
        QStringList list = dialog.selectedFiles();
        if (list.size() != 1) {
            return false;
        } else {
            filePathOut = list[0].toStdString();
            return true;
        }
    } else {
        return false;
    }
}

bool MainWindow::showQuestionDialog(const std::string &title, const std::string &text) {
    return QMessageBox::question(this, title.c_str(), text.c_str()) == QMessageBox::Yes;
}

void MainWindow::quit() {
    exit(0);
}

void MainWindow::setInputText(const std::string &value) {
    if (ui->lineEdit_input->text() != value.c_str())
        ui->lineEdit_input->setText(value.c_str());
}

void MainWindow::setValueText(const std::string &value) {
    ui->label_lastResult->setText(value.c_str());
}

void MainWindow::setKeyPadVisibility(bool visible) {
    ui->widget_keypad->setVisible(visible);
    ui->actionShow_Keypad->setChecked(visible);
}

void MainWindow::setBitViewVisibility(bool visible) {
    ui->widget_bits->setVisible(visible);
    ui->actionShow_Bit_Toggle->setChecked(visible);
}

void MainWindow::setDockVisibility(bool visible) {
    ui->dockWidget->setVisible(visible);
    ui->actionShow_Dock->setChecked(visible);
}

void MainWindow::setActiveDockTab(int tab) {
    ui->tabWidget_2->setCurrentIndex(tab);
}

void MainWindow::setDockPosition(Qt::DockWidgetArea position) {
    if (dockWidgetArea(ui->dockWidget) == position)
        return;
    removeDockWidget(ui->dockWidget);
    addDockWidget(position, ui->dockWidget);
}

void MainWindow::setBitViewContents(const std::bitset<64> &value) {
    for (int i = 0; i < 64; i++) {
        QPushButton &button = getBitButton(i);
        if (value.test(i)) {
            button.setText("1");
        } else {
            button.setText("0");
        }
    }
}

void MainWindow::setBitViewEnabled(bool enabled) {
    ui->widget_bits->setEnabled(enabled);
}

void MainWindow::setNumericSystemsEnabled(bool enabled) {
    ui->tab_numericsystems->setEnabled(enabled);
}

void MainWindow::setDecimalText(const std::string &value) {
    ui->lineEdit_input_decimal->setText(value.c_str());
}

void MainWindow::setHexText(const std::string &value) {
    ui->lineEdit_input_hex->setText(value.c_str());
}

void MainWindow::setOctalText(const std::string &value) {
    ui->lineEdit_input_octal->setText(value.c_str());
}

void MainWindow::setBinaryText(const std::string &value) {
    ui->lineEdit_input_binary->setText(value.c_str());
}

void MainWindow::setHistory(const std::vector<std::pair<std::string, std::string>> &value) {
    std::string text;
    for (auto it = value.begin(); it != value.end(); it++) {
        text.append(it->first);
        text.append(" = ");
        text.append(it->second);
        if (it + 1 != value.end())
            text.append("\n");
    }
    ui->textEdit_history->setText(text.c_str());
    ui->textEdit_history->verticalScrollBar()->setValue(ui->textEdit_history->verticalScrollBar()->maximum());
    ui->textEdit_history->horizontalScrollBar()->setValue(ui->textEdit_history->horizontalScrollBar()->minimum());
}

void MainWindow::setVariableListView(const std::vector<std::pair<std::string, std::string>> &value) {
    int rows = ui->tableWidget_variables->rowCount();
    for (int i = 0; i < rows; i++) {
        ui->tableWidget_variables->removeRow(0);
    }
    ui->tableWidget_variables->insertRow(0);
    for (auto &pair : value) {
        int rowIndex = ui->tableWidget_variables->rowCount();
        ui->tableWidget_variables->insertRow(rowIndex);
        auto *nameItem = new QTableWidgetItem();
        auto *valueItem = new QTableWidgetItem();
        nameItem->setText(pair.first.c_str());
        valueItem->setText(pair.second.c_str());
        ui->tableWidget_variables->setItem(rowIndex, 0, nameItem);
        ui->tableWidget_variables->setItem(rowIndex, 1, valueItem);
    }
}

void MainWindow::setSelectedVariable(int index) {
    if (index != -1)
        ui->tableWidget_variables->selectRow(index + 1);
}

void MainWindow::setConstantsListView(const std::vector<std::pair<std::string, std::string>> &value) {
    int rows = ui->tableWidget_constants->rowCount();
    for (int i = 0; i < rows; i++) {
        ui->tableWidget_constants->removeRow(0);
    }
    ui->tableWidget_constants->insertRow(0);
    for (auto &pair : value) {
        int rowIndex = ui->tableWidget_constants->rowCount();
        ui->tableWidget_constants->insertRow(rowIndex);
        auto *nameItem = new QTableWidgetItem();
        auto *valueItem = new QTableWidgetItem();
        nameItem->setText(pair.first.c_str());
        valueItem->setText(pair.second.c_str());
        ui->tableWidget_constants->setItem(rowIndex, 0, nameItem);
        ui->tableWidget_constants->setItem(rowIndex, 1, valueItem);
    }
}

void MainWindow::setSelectedConstant(int index) {
    if (index != -1)
        ui->tableWidget_constants->selectRow(index + 1);
}

void MainWindow::setFunctionsListView(const std::vector<std::string> &value) {
    int rows = ui->tableWidget_functions->rowCount();
    for (int i = 0; i < rows; i++) {
        ui->tableWidget_functions->removeRow(0);
    }
    ui->tableWidget_functions->insertRow(0);
    for (auto &name : value) {
        int rowIndex = ui->tableWidget_functions->rowCount();
        ui->tableWidget_functions->insertRow(rowIndex);
        auto *nameItem = new QTableWidgetItem();
        nameItem->setText(name.c_str());
        ui->tableWidget_functions->setItem(rowIndex, 0, nameItem);
    }
}

void MainWindow::setSelectedFunction(int index) {
    if (index != -1)
        ui->tableWidget_functions->selectRow(index + 1);
}

void MainWindow::setFunctionArgs(const std::vector<std::string> &value) {
    int numberOfArguments = value.size();
    ui->spinBox_functions_argcount->setValue(numberOfArguments);

    //Use switch to avoid unnecessary hiding (which makes the line edit loose focus)
    switch (numberOfArguments) {
        case 0:
            ui->lineEdit_functions_arg0->setVisible(false);
            ui->lineEdit_functions_arg0->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg0->setText("");
        case 1:
            ui->lineEdit_functions_arg1->setVisible(false);
            ui->lineEdit_functions_arg1->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg1->setText("");
        case 2:
            ui->lineEdit_functions_arg2->setVisible(false);
            ui->lineEdit_functions_arg2->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg2->setText("");
        case 3:
            ui->lineEdit_functions_arg3->setVisible(false);
            ui->lineEdit_functions_arg3->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg3->setText("");
        case 4:
            ui->lineEdit_functions_arg4->setVisible(false);
            ui->lineEdit_functions_arg4->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg4->setText("");
        default:
            break;
    }

    switch (numberOfArguments) {
        case 5:
            ui->lineEdit_functions_arg4->setVisible(true);
            ui->lineEdit_functions_arg4->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg4->setText(value[4].c_str());
        case 4:
            ui->lineEdit_functions_arg3->setVisible(true);
            ui->lineEdit_functions_arg3->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg3->setText(value[3].c_str());
        case 3:
            ui->lineEdit_functions_arg2->setVisible(true);
            ui->lineEdit_functions_arg2->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg2->setText(value[2].c_str());
        case 2:
            ui->lineEdit_functions_arg1->setVisible(true);
            ui->lineEdit_functions_arg1->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg1->setText(value[1].c_str());
        case 1:
            ui->lineEdit_functions_arg0->setVisible(true);
            ui->lineEdit_functions_arg0->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
            ui->lineEdit_functions_arg0->setText(value[0].c_str());
        default:
            break;
    }
}

void MainWindow::setFunctionBody(const std::string &value) {
    ui->textEdit_functions_body->setText(value.c_str());
}

void MainWindow::setFunctionArgsSpinBoxEnabled(bool enabled) {
    ui->spinBox_functions_argcount->setEnabled(enabled);
}

void MainWindow::setFunctionBodyEnabled(bool enabled) {
    ui->textEdit_functions_body->setEnabled(enabled);
}

void MainWindow::setScriptsListView(const std::vector<std::string> &value) {
    int rows = ui->tableWidget_scripts->rowCount();
    for (int i = 0; i < rows; i++) {
        ui->tableWidget_scripts->removeRow(0);
    }
    ui->tableWidget_scripts->insertRow(0);
    for (auto &name : value) {
        int rowIndex = ui->tableWidget_scripts->rowCount();
        ui->tableWidget_scripts->insertRow(rowIndex);
        auto *nameItem = new QTableWidgetItem();
        nameItem->setText(name.c_str());
        ui->tableWidget_scripts->setItem(rowIndex, 0, nameItem);
    }
}

void MainWindow::setSelectedScript(int index) {
    if (index != -1)
        ui->tableWidget_scripts->selectRow(index + 1);
}

void MainWindow::setScriptBody(const std::string &value) {
    ui->textEdit_scripts->setText(value.c_str());
}

void MainWindow::setScriptBodyEnabled(bool enabled) {
    ui->textEdit_scripts->setEnabled(enabled);
}

void MainWindow::setScriptEnableArgs(bool value) {
    ui->checkBox_scripts_enableargs->setChecked(value);
}

void MainWindow::setScriptEnableArgsEnabled(bool value) {
    ui->checkBox_scripts_enableargs->setEnabled(value);
}

void MainWindow::setWindowSize(QSize size) {
    resize(size);
}
//-View

//-SLOTS
void MainWindow::onNumericSystemsFinishEditing() {
    auto *sen = sender();
    assert(sen != nullptr);

    auto &s = dynamic_cast<QLineEdit &>(*sen);
    if (&s == ui->lineEdit_input_decimal) {
        emit signalDecimalSubmit(s.text());
    } else if (&s == ui->lineEdit_input_hex) {
        emit signalHexSubmit(s.text());
    } else if (&s == ui->lineEdit_input_octal) {
        emit signalOctalSubmit(s.text());
    } else if (&s == ui->lineEdit_input_binary) {
        emit signalBinarySubmit(s.text());
    } else {
        assert(false);
    }
}

void MainWindow::onNumPadKeyPressed() {
    auto *s = sender();
    assert(s != nullptr);
    emit signalNumPadKeyPressed(getKeyFromName(s->objectName()));
}

void MainWindow::onBitViewKeyPressed() {
    auto *s = sender();
    assert(s != nullptr);
    emit signalBitViewKeyPressed(getBitIndex(s->objectName()));
}

void MainWindow::onVariablesSelectionChanged() {
    auto selection = ui->tableWidget_variables->selectionModel()->selectedRows();
    if (selection.empty()) {
        emit signalSelectedVariableChanged(-1);
    } else {
        int row = selection[0].row();
        emit signalSelectedVariableChanged(row == 0 ? -1 : row - 1);
    }
}

void MainWindow::onVariablesCellChanged(int row, int column) {
    QTableWidgetItem *nameItem = ui->tableWidget_variables->item(row, 0);
    QTableWidgetItem *valueItem = ui->tableWidget_variables->item(row, 1);

    std::string name;
    std::string value;

    if (nameItem != nullptr)
        name = nameItem->text().toStdString();
    else
        name = "";

    if (valueItem != nullptr)
        value = valueItem->text().toStdString();
    else
        value = "0";

    emit signalVariableChanged(name, value);
}

void MainWindow::onConstantsSelectionChanged() {
    auto selection = ui->tableWidget_constants->selectionModel()->selectedRows();
    if (selection.empty()) {
        emit signalSelectedConstantChanged(-1);
    } else {
        int row = selection[0].row();
        emit signalSelectedConstantChanged(row == 0 ? -1 : row - 1);
    }
}

void MainWindow::onConstantsCellChanged(int row, int column) {
    QTableWidgetItem *nameItem = ui->tableWidget_constants->item(row, 0);
    QTableWidgetItem *valueItem = ui->tableWidget_constants->item(row, 1);

    std::string name;
    std::string value;

    if (nameItem != nullptr)
        name = nameItem->text().toStdString();
    else
        name = "";

    if (valueItem != nullptr)
        value = valueItem->text().toStdString();
    else
        value = "0";

    emit signalConstantChanged(name, value);
}

void MainWindow::onFunctionsSelectionChanged() {
    auto selection = ui->tableWidget_functions->selectionModel()->selectedRows();
    if (selection.empty()) {
        emit signalSelectedFunctionChanged(-1);
    } else {
        int row = selection[0].row();
        emit signalSelectedFunctionChanged(row == 0 ? -1 : row - 1);
    }
}

void MainWindow::onFunctionsCellChanged(int row, int column) {
    QTableWidgetItem *nameItem = ui->tableWidget_functions->item(row, 0);

    std::string name;

    if (nameItem != nullptr)
        name = nameItem->text().toStdString();
    else
        name = "";

    emit signalFunctionNameChanged(name);
}

void MainWindow::onFunctionsArgCountValueChanged(int value) {
    std::vector<std::string> args;
    switch (value) {
        case 5:
            args.emplace_back(ui->lineEdit_functions_arg4->text().toStdString());
        case 4:
            args.emplace_back(ui->lineEdit_functions_arg3->text().toStdString());
        case 3:
            args.emplace_back(ui->lineEdit_functions_arg2->text().toStdString());
        case 2:
            args.emplace_back(ui->lineEdit_functions_arg1->text().toStdString());
        case 1:
            args.emplace_back(ui->lineEdit_functions_arg0->text().toStdString());
            break;
    }
    std::reverse(args.begin(), args.end());
    emit signalFunctionArgsChanged(args);
}

void MainWindow::onFunctionsArgEditingFinished() {
    int argCount = ui->spinBox_functions_argcount->value();
    std::vector<std::string> args;
    switch (argCount) {
        case 5:
            args.emplace_back(ui->lineEdit_functions_arg4->text().toStdString());
        case 4:
            args.emplace_back(ui->lineEdit_functions_arg3->text().toStdString());
        case 3:
            args.emplace_back(ui->lineEdit_functions_arg2->text().toStdString());
        case 2:
            args.emplace_back(ui->lineEdit_functions_arg1->text().toStdString());
        case 1:
            args.emplace_back(ui->lineEdit_functions_arg0->text().toStdString());
            break;
    }
    std::reverse(args.begin(), args.end());
    emit signalFunctionArgsChanged(args);
}

void MainWindow::onFunctionsBodyTextChanged() {
    emit signalFunctionBodyChanged(ui->textEdit_functions_body->toPlainText().toStdString());
}

void MainWindow::onScriptsSelectionChanged() {
    auto selection = ui->tableWidget_scripts->selectionModel()->selectedRows();
    if (selection.empty()) {
        emit signalSelectedScriptChanged(-1);
    } else {
        int row = selection[0].row();
        emit signalSelectedScriptChanged(row == 0 ? -1 : row - 1);
    }
}

void MainWindow::onScriptsCellChanged(int row, int column) {
    QTableWidgetItem *nameItem = ui->tableWidget_scripts->item(row, 0);

    std::string name;

    if (nameItem != nullptr)
        name = nameItem->text().toStdString();
    else
        name = "";

    emit signalScriptNameChanged(name);
}

void MainWindow::onScriptsBodyTextChanged() {
    emit signalScriptBodyChanged(ui->textEdit_scripts->toPlainText().toStdString());
}
//-SLOTS

QPushButton &MainWindow::getBitButton(int bitIndex) {
    assert(bitIndex >= 0);
    assert(bitIndex < 64);

    std::string name = OBJECT_NAME_PREFIX_BITS;
    if (bitIndex < 10) {
        name += "0";
    }
    name += std::to_string(bitIndex);

    auto *p = ui->widget_bits->findChild<QPushButton *>(QString::fromStdString(name));

    assert(p != nullptr);

    return *p;
}

int MainWindow::getBitIndex(const QString &name) {
    return std::stoi(name.toStdString().substr(strlen(OBJECT_NAME_PREFIX_BITS)));
}

QPushButton &MainWindow::getKeyPadButton(NumPadKey key) {
    auto *ret = ui->centralwidget->findChild<QPushButton *>(getNameFromKey(key));
    if (ret == nullptr)
        throw std::runtime_error(QString("Key ")
                                         .append(std::to_string(key).c_str())
                                         .append(" not found").toStdString());
    return *ret;
}

QString MainWindow::getNameFromKey(NumPadKey key) {
    switch (key) {
        case KEY_0:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("0");
        case KEY_1:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("1");
        case KEY_2:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("2");
        case KEY_3:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("3");
        case KEY_4:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("4");
        case KEY_5:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("5");
        case KEY_6:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("6");
        case KEY_7:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("7");
        case KEY_8:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("8");
        case KEY_9:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("9");
        case KEY_A:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("a");
        case KEY_B:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("b");
        case KEY_C:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("c");
        case KEY_D:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("d");
        case KEY_E:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("e");
        case KEY_F:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("f");
        case KEY_DOT:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("dot");
        case KEY_DIVIDE:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("divide");
        case KEY_MULTIPLY:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("multiply");
        case KEY_ADD:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("add");
        case KEY_SUBTRACT:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("subtract");
        case KEY_OPENBRACKET:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("openbracket");
        case KEY_CLOSEBRACKET:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("closebracket");
        case KEY_EQUAL:
            return QString(OBJECT_NAME_PREFIX_KEYPAD).append("equal");
        default:
            throw std::runtime_error("Unsupported key");
    }
}

NumPadKey MainWindow::getKeyFromName(const QString &name) {
    for (int k = START + 1; k < END; k++) {
        if (getNameFromKey((NumPadKey) k).toStdString() == name.toStdString()) {
            return (NumPadKey) k;
        }
    }
    throw std::runtime_error(QString("Name not found ").append(name).toStdString());
}