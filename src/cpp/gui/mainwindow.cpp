#include "mainwindow.hpp"

#include <QScrollBar>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>

#include "ui_mainwindow.h"

#include "settingsdialog.hpp"
#include "numeralrep.hpp"
#include "common.hpp"
#include "settings.hpp"
#include "serializer.hpp"

#define OBJECT_NAME_PREFIX_BITS "pushButton_bit_"
#define OBJECT_NAME_PREFIX_KEYPAD "pushButton_kp_"

#define SYMBOLTABLE_FILENAME "/symbols.json"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), calc() {
    ui->setupUi(this);
    connectUI();
    loadState();
    ui->lineEdit_input->setFocus();
    ui->lineEdit_functions_arg0->setVisible(false);
    ui->lineEdit_functions_arg1->setVisible(false);
    ui->lineEdit_functions_arg2->setVisible(false);
    ui->lineEdit_functions_arg3->setVisible(false);
    ui->lineEdit_functions_arg4->setVisible(false);
    ui->textEdit_functions_body->setEnabled(false);
    ui->spinBox_functions_argcount->setEnabled(false);
    ui->textEdit_scripts->setEnabled(false);
}

MainWindow::~MainWindow() {
    disconnectUI();
    delete ui;
}

void MainWindow::slotInputReturnPressed() {
    if (ui->lineEdit_input->text().length() > 0)
        onInputCommit(ui->lineEdit_input->text().toStdString());
}

void MainWindow::slotInputDecimalEditingFinished() {
    //Only accepts a single literal in decimal
    std::string s = ui->lineEdit_input_decimal->text().toStdString();
    if (s.rfind("Error", 0) != 0) {
        try {
            long double v = NumeralRep::fromDecimal(s);
            if (expressionHistory.empty() || v != expressionHistory.back().second)
                onInputCommit(std::to_string(v));
            else
                setToolboxValues(expressionHistory.back().second);
        }
        catch (std::invalid_argument &e) {
            ui->statusbar->showMessage(QString::fromStdString(e.what()));
            if (!expressionHistory.empty())
                setToolboxValues(expressionHistory.back().second);
            else
                onInputCommit("0");
        }
    }
}

void MainWindow::slotInputHexEditingFinished() {
    //Only accepts a single literal in hex
    std::string s = ui->lineEdit_input_hex->text().toStdString();
    if (s.rfind("Error", 0) != 0) {
        try {
            long double v = NumeralRep::fromHex(s);
            if (expressionHistory.empty() || v != expressionHistory.back().second)
                onInputCommit(std::to_string(v));
            else
                setToolboxValues(expressionHistory.back().second);
        }
        catch (std::invalid_argument &e) {
            ui->statusbar->showMessage(QString::fromStdString(e.what()));
            if (!expressionHistory.empty())
                setToolboxValues(expressionHistory.back().second);
            else
                onInputCommit("0");
        }
    }
}

void MainWindow::slotInputOctalEditingFinished() {
    //Only accepts a single literal in octal
    std::string s = ui->lineEdit_input_octal->text().toStdString();
    if (s.rfind("Error", 0) != 0) {
        try {
            long double v = NumeralRep::fromOctal(s);
            if (expressionHistory.empty() || v != expressionHistory.back().second)
                onInputCommit(std::to_string(v));
            else
                setToolboxValues(expressionHistory.back().second);
        } catch (std::invalid_argument &e) {
            ui->statusbar->showMessage(QString::fromStdString(e.what()));
            if (!expressionHistory.empty())
                setToolboxValues(expressionHistory.back().second);
            else
                onInputCommit("0");
        }
    }
}

void MainWindow::slotInputBinaryEditingFinished() {
    //Only accepts a single literal in binary
    std::string s = ui->lineEdit_input_binary->text().toStdString();
    if (s.rfind("Error", 0) != 0) {
        try {
            long double v = NumeralRep::fromBinary(s);
            if (expressionHistory.empty() || v != expressionHistory.back().second)
                onInputCommit(std::to_string(v));
            else
                setToolboxValues(expressionHistory.back().second);
        }
        catch (std::invalid_argument &e) {
            ui->statusbar->showMessage(QString::fromStdString(e.what()));
            if (!expressionHistory.empty())
                setToolboxValues(expressionHistory.back().second);
            else
                onInputCommit("0");
        }
    }
}

void MainWindow::slotNumPadClick() {
    QObject *obj = sender();

    NumPadKey key = getKeyFromName(obj->objectName());

    if (key == NumPadKey::KEY_EQUAL) {
        slotInputReturnPressed();
    } else {
        onKeyPadInput(key);
    }
}

void MainWindow::slotBitClick() {
    QObject *obj = sender();

    int bitIndex = getBitIndex(obj->objectName());
    bitViewValue.flip(bitIndex);

    onBitViewCommit(bitViewValue);
}

void MainWindow::connectUI() {
    connect(ui->lineEdit_input, SIGNAL(returnPressed()), this, SLOT(slotInputReturnPressed()));

    connect(ui->lineEdit_input_decimal, SIGNAL(editingFinished()), this, SLOT(slotInputDecimalEditingFinished()));
    connect(ui->lineEdit_input_hex, SIGNAL(editingFinished()), this, SLOT(slotInputHexEditingFinished()));
    connect(ui->lineEdit_input_octal, SIGNAL(editingFinished()), this, SLOT(slotInputOctalEditingFinished()));
    connect(ui->lineEdit_input_binary, SIGNAL(editingFinished()), this, SLOT(slotInputBinaryEditingFinished()));

    for (int i = 0; i < 64; i++) {
        if (!connect(getBitButton(i), SIGNAL(clicked()), this, SLOT(slotBitClick())))
            throw std::runtime_error("Failed to connect bit button");
    }

    for (int k = START + 1; k < END; k++) {
        if (!connect(getKeyPadButton((NumPadKey) k), SIGNAL(clicked()), this, SLOT(slotNumPadClick())))
            throw std::runtime_error("Failed to connect keypad");
    }

    connect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(slot_dockWidget_visibilityChanged(bool)));

    connect(ui->tableWidget_scripts, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
            SLOT(slot_tableWidget_scripts_currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    connect(ui->tableWidget_scripts, SIGNAL(itemChanged(QTableWidgetItem * )), this,
            SLOT(slot_tableWidget_scripts_itemChanged(QTableWidgetItem * )));

    connect(ui->textEdit_scripts, SIGNAL(textChanged()), this, SLOT(slot_textEdit_scripts_textChanged()));

    connect(ui->tableWidget_functions, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
            SLOT(slotFunctionTableWidgetCurrentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    connect(ui->tableWidget_functions, SIGNAL(itemChanged(QTableWidgetItem * )), this,
            SLOT(slotFunctionTableWidgetItemChanged(QTableWidgetItem * )));

    connect(ui->spinBox_functions_argcount, SIGNAL(valueChanged(int)), this, SLOT(slotFunctionArgCountChange(int)));
    connect(ui->lineEdit_functions_arg0, SIGNAL(textChanged(const QString &)), this,
            SLOT(slotFunctionArgChange0(const QString &)));
    connect(ui->lineEdit_functions_arg1, SIGNAL(textChanged(const QString &)), this,
            SLOT(slotFunctionArgChange1(const QString &)));
    connect(ui->lineEdit_functions_arg2, SIGNAL(textChanged(const QString &)), this,
            SLOT(slotFunctionArgChange2(const QString &)));
    connect(ui->lineEdit_functions_arg3, SIGNAL(textChanged(const QString &)), this,
            SLOT(slotFunctionArgChange3(const QString &)));
    connect(ui->lineEdit_functions_arg4, SIGNAL(textChanged(const QString &)), this,
            SLOT(slotFunctionArgChange4(const QString &)));
    connect(ui->textEdit_functions_body, SIGNAL(textChanged()), this, SLOT(slotFunctionBodyChange()));

    connect(ui->tableWidget_variables, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
            SLOT(slotVariablesTableWidget_currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    connect(ui->tableWidget_variables, SIGNAL(itemChanged(QTableWidgetItem * )), this,
            SLOT(slotVariablesTableWidget_itemChanged(QTableWidgetItem * )));

    connect(ui->tableWidget_constants, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
            SLOT(slotConstantsTableWidget_currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    connect(ui->tableWidget_constants, SIGNAL(itemChanged(QTableWidgetItem * )), this,
            SLOT(slotConstantsTableWidget_itemChanged(QTableWidgetItem * )));
}

void MainWindow::disconnectUI() {
    disconnect(ui->lineEdit_input, SIGNAL(returnPressed()), this, SLOT(slotInputReturnPressed()));

    disconnect(ui->lineEdit_input_decimal, SIGNAL(editingFinished()), this, SLOT(slotInputDecimalEditingFinished()));
    disconnect(ui->lineEdit_input_hex, SIGNAL(editingFinished()), this, SLOT(slotInputHexEditingFinished()));
    disconnect(ui->lineEdit_input_octal, SIGNAL(editingFinished()), this, SLOT(slotInputOctalEditingFinished()));
    disconnect(ui->lineEdit_input_binary, SIGNAL(editingFinished()), this, SLOT(slotInputBinaryEditingFinished()));

    for (int i = 0; i < 64; i++) {
        disconnect(getBitButton(i), SIGNAL(clicked()), this, SLOT(slotBitClick()));
    }

    for (int k = START + 1; k < END; k++) {
        disconnect(getKeyPadButton((NumPadKey) k), SIGNAL(clicked()), this, SLOT(slotNumPadClick()));
    }

    disconnect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(slot_dockWidget_visibilityChanged(bool)));

    disconnect(ui->tableWidget_scripts, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
               SLOT(slot_tableWidget_scripts_currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    disconnect(ui->tableWidget_scripts, SIGNAL(itemChanged(QTableWidgetItem * )), this,
               SLOT(slot_tableWidget_scripts_itemChanged(QTableWidgetItem * )));

    disconnect(ui->textEdit_scripts, SIGNAL(textChanged()), this, SLOT(slot_textEdit_scripts_textChanged()));

    disconnect(ui->tableWidget_functions, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
               SLOT(slotFunctionTableWidgetCurrentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    disconnect(ui->tableWidget_functions, SIGNAL(itemChanged(QTableWidgetItem * )), this,
               SLOT(slotFunctionTableWidgetItemChanged(QTableWidgetItem * )));

    disconnect(ui->spinBox_functions_argcount, SIGNAL(valueChanged(int)), this, SLOT(slotFunctionArgCountChange(int)));
    disconnect(ui->lineEdit_functions_arg0, SIGNAL(textChanged(const QString &)), this,
               SLOT(slotFunctionArgChange0(const QString &)));
    disconnect(ui->lineEdit_functions_arg1, SIGNAL(textChanged(const QString &)), this,
               SLOT(slotFunctionArgChange1(const QString &)));
    disconnect(ui->lineEdit_functions_arg2, SIGNAL(textChanged(const QString &)), this,
               SLOT(slotFunctionArgChange2(const QString &)));
    disconnect(ui->lineEdit_functions_arg3, SIGNAL(textChanged(const QString &)), this,
               SLOT(slotFunctionArgChange3(const QString &)));
    disconnect(ui->lineEdit_functions_arg4, SIGNAL(textChanged(const QString &)), this,
               SLOT(slotFunctionArgChange4(const QString &)));
    disconnect(ui->textEdit_functions_body, SIGNAL(textChanged()), this, SLOT(slotFunctionBodyChange()));

    disconnect(ui->tableWidget_variables, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
               SLOT(slotVariablesTableWidget_currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    disconnect(ui->tableWidget_variables, SIGNAL(itemChanged(QTableWidgetItem * )), this,
               SLOT(slotVariablesTableWidget_itemChanged(QTableWidgetItem * )));

    disconnect(ui->tableWidget_constants, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )), this,
               SLOT(slotConstantsTableWidget_currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )));
    disconnect(ui->tableWidget_constants, SIGNAL(itemChanged(QTableWidgetItem * )), this,
               SLOT(slotConstantsTableWidget_itemChanged(QTableWidgetItem * )));
}

bool MainWindow::parseExpression(const std::string &expr) {
    try {
        long double result = calc.evaluate(expr, symbols);

        ui->label_lastResult->setText(QString::fromStdString(NumeralRep::toDecimal(result)));
        setInput(result);
        setToolboxValues(result);

        if (!hasFraction(result)
            && result >= 0
            && result <= (long double) std::numeric_limits<unsigned long long>::max()) {
            auto v = (unsigned long long) result;
            bitViewValue = std::bitset<64>(v);
            setBitView(bitViewValue);
        } else {
            hideBitView();
        }

        expressionHistory.emplace_back(expr, result);
        setHistory(expressionHistory);

        std::string s = expr;
        s += " = ";
        s += NumeralRep::toDecimal(result);

        ui->statusbar->showMessage(QString::fromStdString(s));

        return true;
    }
    catch (std::runtime_error &e) {
        std::string s = "Error: ";
        s += e.what();
        s += " ,For Expression: ";
        s += expr;
        ui->statusbar->showMessage(QString::fromStdString(s));

        if (!expressionHistory.empty())
            setToolboxValues(expressionHistory.back().second);
        else
            setToolboxValues(0);
    }
    return false;
}

void MainWindow::setBitView(std::bitset<64> value) {
    ui->widget_bits->setDisabled(false);
    for (int i = 0; i < 64; i++) {
        QPushButton *btn = getBitButton(i);
        if (value.test(i)) {
            btn->setText("1");
        } else {
            btn->setText("0");
        }
    }
}

void MainWindow::hideBitView() {
    for (int i = 0; i < 64; i++) {
        bitViewValue.set(i, false);
    }
    setBitView(bitViewValue);
    ui->widget_bits->setDisabled(true);
}

void MainWindow::setInput(const QString &s) {
    ui->lineEdit_input->setText(s);
}

void MainWindow::setInput(const std::string &s) {
    setInput(QString::fromStdString(s));
}

void MainWindow::setInput(long double value) {
    switch (inputLiteralParseMode) {
        case DECIMAL:
            setInput(NumeralRep::toDecimal(value));
            break;
        case HEXADECIMAL:
            setInput(NumeralRep::toHex(value));
            break;
        case OCTAL:
            setInput(NumeralRep::toOctal(value));
            break;
        case BINARY:
            setInput(NumeralRep::toBinary(value));
            break;
    }
}

void MainWindow::setToolboxValues(long double value) {
    if (hasFraction(value)) {
        ui->lineEdit_input_decimal->setText("");
        ui->lineEdit_input_hex->setText("");
        ui->lineEdit_input_octal->setText("");
        ui->lineEdit_input_binary->setText("");

        ui->tab_numericsystems->setEnabled(false);
    } else {
        ui->lineEdit_input_decimal->setText(QString::fromStdString(NumeralRep::toDecimal(value)));
        ui->lineEdit_input_hex->setText(QString::fromStdString(NumeralRep::toHex(value)));
        ui->lineEdit_input_octal->setText(QString::fromStdString(NumeralRep::toOctal(value)));
        ui->lineEdit_input_binary->setText(QString::fromStdString(NumeralRep::toBinary(value)));

        ui->tab_numericsystems->setEnabled(true);
    }
}

void MainWindow::setHistory(std::vector<std::pair<std::string, long double>> history) {
    QString text;
    for (std::size_t i = 0; i < history.size(); i++) {
        std::pair<std::string, long double> &pair = history[i];
        text += QString::fromStdString(pair.first);
        text += " = ";
        text += QString::fromStdString(NumeralRep::toDecimal(pair.second));
        if (i < history.size() - 1)
            text += "\n";
    }
    ui->textEdit_history->setText(text);
    ui->textEdit_history->verticalScrollBar()->setValue(ui->textEdit_history->verticalScrollBar()->maximum());
    ui->textEdit_history->horizontalScrollBar()->setValue(ui->textEdit_history->horizontalScrollBar()->minimum());
}

void MainWindow::onInputCommit(const std::string &input) {
    parseExpression(input);
}

void MainWindow::onToolboxCommit(const std::string &input) {
    parseExpression(input);
}

void MainWindow::onKeyPadInput(NumPadKey key) {
    std::string input = getInputFromkey(key);
    QString t = ui->lineEdit_input->text();
    t += input.c_str();
    ui->lineEdit_input->setText(t);
}

void MainWindow::onNumeralSystemChange(NumeralSystem system) {
    /*Problem:
     * When using non decimal literals in an expression,
     * the literals in the expression have to be converted to decimal or escaped
     * before passing the expression to the calculator engine.
    */
}

void MainWindow::onBitViewCommit(std::bitset<64> value) {
    bitViewValue = value;
    assert(parseExpression(std::to_string(value.to_ullong())));
}

QPushButton *MainWindow::getBitButton(int bitIndex) {
    assert(bitIndex >= 0);
    assert(bitIndex < 64);

    std::string name = OBJECT_NAME_PREFIX_BITS;
    if (bitIndex < 10) {
        name += "0";
    }
    name += std::to_string(bitIndex);

    auto *p = ui->widget_bits->findChild<QPushButton *>(QString::fromStdString(name));

    assert(p != nullptr);

    return p;
}

int MainWindow::getBitIndex(const QString &name) {
    return std::stoi(name.toStdString().substr(strlen(OBJECT_NAME_PREFIX_BITS)));
}

QPushButton *MainWindow::getKeyPadButton(NumPadKey key) {
    auto *ret = ui->centralwidget->findChild<QPushButton *>(getNameFromKey(key));
    if (ret == nullptr)
        throw std::runtime_error(QString("Key ")
                                         .append(std::to_string(key).c_str())
                                         .append(" not found").toStdString());
    return ret;
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

std::string MainWindow::getInputFromkey(NumPadKey key) {
    switch (key) {
        case KEY_0:
            return "0";
        case KEY_1:
            return "1";
        case KEY_2:
            return "2";
        case KEY_3:
            return "3";
        case KEY_4:
            return "4";
        case KEY_5:
            return "5";
        case KEY_6:
            return "6";
        case KEY_7:
            return "7";
        case KEY_8:
            return "8";
        case KEY_9:
            return "9";
            /*case KEY_A:
                  return "A";
              case KEY_B:
                  return "B";
              case KEY_C:
                  return "C";
              case KEY_D:
                  return "D";
              case KEY_E:
                  return "E";
              case KEY_F:
                  return "F";*/
        case KEY_DOT:
            return ".";
        case KEY_DIVIDE:
            return "/";
        case KEY_MULTIPLY:
            return "*";
        case KEY_ADD:
            return "+";
        case KEY_SUBTRACT:
            return "-";
        case KEY_OPENBRACKET:
            return "(";
        case KEY_CLOSEBRACKET:
            return ")";
        default:
            throw std::runtime_error("Unsupported key");
    }
}

void MainWindow::on_actionExit_triggered() {
    saveState();
    settings.sync();
    exit(0);
}

void MainWindow::on_actionAbout_Qt_Calculator_triggered() {
    QMessageBox::about(this, "About " + QApplication::applicationDisplayName(),
                       QApplication::applicationDisplayName()
                       + " " + QApplication::applicationVersion()
                       + "\n\n"
                       + QString(R"LLL(Source: https://github.com/nepomok/qt-calc

Powered by https://github.com/ArashPartow/exprtk
)LLL"));
}

void MainWindow::on_actionShow_Keypad_toggled(bool arg1) {
    ui->widget_keypad->setVisible(arg1);
}

void MainWindow::on_actionShow_Bit_Toggle_toggled(bool arg1) {
    ui->widget_bits->setVisible(arg1);
}

void MainWindow::on_actionShow_Dock_toggled(bool arg1) {
    ui->dockWidget->setVisible(arg1);
}

void MainWindow::loadState() {
    disconnectUI();

    variableMapping.clear();
    constantsMapping.clear();
    functionMapping.clear();
    scriptMapping.clear();
    currentScript = nullptr;
    currentFunction = nullptr;

    ui->tableWidget_scripts->setRowCount(0);
    ui->tableWidget_scripts->setRowCount(1);
    ui->tableWidget_functions->setRowCount(0);
    ui->tableWidget_functions->setRowCount(1);
    ui->tableWidget_variables->setRowCount(0);
    ui->tableWidget_variables->setRowCount(1);
    ui->tableWidget_constants->setRowCount(0);
    ui->tableWidget_constants->setRowCount(1);

    ui->lineEdit_functions_arg0->hide();
    ui->lineEdit_functions_arg1->hide();
    ui->lineEdit_functions_arg2->hide();
    ui->lineEdit_functions_arg3->hide();
    ui->lineEdit_functions_arg4->hide();
    ui->spinBox_functions_argcount->setValue(0);
    ui->textEdit_functions_body->setText("");
    ui->textEdit_functions_body->setEnabled(false);

    ui->textEdit_scripts->setText("");
    ui->textEdit_scripts->setEnabled(false);

    if (settings.value(SETTINGS_SAVE, true).toBool()) {
        bool v = settings.value(SETTINGS_SHOW_BITTOGGLE, true).toBool();
        ui->actionShow_Bit_Toggle->setChecked(v);
        ui->widget_bits->setVisible(v);

        v = settings.value(SETTINGS_SHOW_KEYPAD, true).toBool();
        ui->actionShow_Keypad->setChecked(v);
        ui->widget_keypad->setVisible(v);

        resize(settings.value(SETTINGS_SIZE, this->size()).toSize());

        Qt::DockWidgetArea dockArea = (Qt::DockWidgetArea) (settings.value(SETTINGS_DOCKPOS,
                                                                           Qt::DockWidgetArea::RightDockWidgetArea).toInt());

        addDockWidget(dockArea, ui->dockWidget);

        ui->dockWidget->setVisible(settings.value(SETTINGS_SHOW_DOCK, true).toBool());

        QString appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!QDir(appDir).exists())
            QDir().mkdir(appDir);

        QFile symbolFile(appDir.append(SYMBOLTABLE_FILENAME));

        if (symbolFile.exists()) {
            //TODO: Symbol table serialization error handling
            symbolFile.open(QFile::ReadOnly);

            QTextStream symbolStream(&symbolFile);
            symbols = Serializer::deserializeTable(symbolStream.readAll().toStdString());

            for (auto &p : symbols.variables) {
                int index = ui->tableWidget_variables->rowCount();
                ui->tableWidget_variables->insertRow(index);
                ui->tableWidget_variables->setItem(index, 0, new QTableWidgetItem());
                ui->tableWidget_variables->setItem(index, 1, new QTableWidgetItem());
                QTableWidgetItem *item = ui->tableWidget_variables->item(index, 0);
                item->setText(p.first.c_str());
                variableMapping[item] = p.first;
                item = ui->tableWidget_variables->item(index, 1);
                item->setText(std::to_string(p.second).c_str());
            }

            for (auto &p : symbols.constants) {
                int index = ui->tableWidget_constants->rowCount();
                ui->tableWidget_constants->insertRow(index);
                ui->tableWidget_constants->setItem(index, 0, new QTableWidgetItem());
                ui->tableWidget_constants->setItem(index, 1, new QTableWidgetItem());
                QTableWidgetItem *item = ui->tableWidget_constants->item(index, 0);
                item->setText(p.first.c_str());
                constantsMapping[item] = p.first;
                item = ui->tableWidget_constants->item(index, 1);
                item->setText(std::to_string(p.second).c_str());
            }

            for (auto &p : symbols.functions) {
                int index = ui->tableWidget_functions->rowCount();
                ui->tableWidget_functions->insertRow(index);
                ui->tableWidget_functions->setItem(index, 0, new QTableWidgetItem());
                QTableWidgetItem *item = ui->tableWidget_functions->item(index, 0);
                item->setText(p.first.c_str());
                functionMapping[item] = p.first;
            }

            for (auto &p : symbols.scripts) {
                int index = ui->tableWidget_scripts->rowCount();
                ui->tableWidget_scripts->insertRow(index);
                ui->tableWidget_scripts->setItem(index, 0, new QTableWidgetItem());
                QTableWidgetItem *item = ui->tableWidget_scripts->item(index, 0);
                item->setText(p.first.c_str());
                scriptMapping[item] = p.first;
            }
        }
    } else {
        ui->actionShow_Bit_Toggle->setChecked(true);
        ui->actionShow_Dock->setChecked(true);
        ui->actionShow_Keypad->setChecked(true);

        ui->widget_keypad->setVisible(true);
        ui->dockWidget->setVisible(true);
        ui->widget_bits->setVisible(true);

        ui->dockWidget->setVisible(true);
    }
    connectUI();
}

void MainWindow::saveState() {
    if (settings.value(SETTINGS_SAVE, true).toBool()) {
        settings.setValue(SETTINGS_SHOW_BITTOGGLE, ui->widget_bits->isVisible());
        settings.setValue(SETTINGS_SHOW_DOCK, ui->dockWidget->isVisible());
        settings.setValue(SETTINGS_SHOW_KEYPAD, ui->widget_keypad->isVisible());
        settings.setValue(SETTINGS_SIZE, this->size());
        settings.setValue(SETTINGS_DOCKPOS, dockWidgetArea(ui->dockWidget));

        QString appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!QDir(appDir).exists())
            QDir().mkdir(appDir);

        QFile symbolFile(appDir.append(SYMBOLTABLE_FILENAME));
        symbolFile.open(QFile::WriteOnly | QFile::Truncate);

        QTextStream symbolStream(&symbolFile);
        symbolStream << QString(Serializer::serializeTable(symbols).c_str());
        symbolStream.flush();
    }
}

void MainWindow::slot_dockWidget_visibilityChanged(bool visible) {
    ui->actionShow_Dock->setChecked(visible);
}

void MainWindow::on_actionSettings_triggered() {
    saveState();
    SettingsDialog dialog(settings);
    dialog.exec();
    loadState();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveState();
    settings.sync();
    exit(0);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void MainWindow::slot_tableWidget_scripts_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    currentScript = current;
    if (current == nullptr) {
        ui->textEdit_scripts->setText("");
        ui->textEdit_scripts->setEnabled(false);
    } else if (current->row() != 0) {
        ui->textEdit_scripts->setText(symbols.scripts[scriptMapping[current]].body.c_str());
        ui->textEdit_scripts->setEnabled(true);
    } else {
        //Default entry
        ui->textEdit_scripts->setText("");
        ui->textEdit_scripts->setEnabled(false);
    }
}

void MainWindow::slot_tableWidget_scripts_itemChanged(QTableWidgetItem *item) {
    if (item == nullptr)
        return;
    disconnectUI();
    std::string scriptName = item->text().toStdString();
    if (item->row() == 0) {
        //Create
        currentScript = item;
        if (scriptName.empty()) {
            QMessageBox::warning(this, "Error", "Script name cannot be empty.");
        } else if (symbols.scripts.find(scriptName) != symbols.scripts.end()) {
            QMessageBox::warning(this, "Error", "Script with name " + QString(scriptName.c_str()) + " already exists.");
        } else {
            scriptMapping[item] = scriptName;
            symbols.scripts[scriptName].body = "";
            ui->tableWidget_scripts->insertRow(0);
            ui->tableWidget_scripts->setCurrentItem(
                    ui->tableWidget_scripts->item(1, 0)
            );
            ui->textEdit_scripts->setEnabled(true);
            ui->textEdit_scripts->setText(symbols.scripts[scriptMapping[item]].body.c_str());
        }
    } else {
        //Update name
        if (scriptName.empty()) {
            QMessageBox::warning(this, "Error", "Script name cannot be empty.");
        } else if (scriptName != scriptMapping[item]
                   && symbols.scripts.find(scriptName) != symbols.scripts.end()) {
            QMessageBox::warning(this, "Error", "Script with name " + QString(scriptName.c_str()) + " already exists.");
            item->setText(scriptMapping[item].c_str());
        } else {
            symbols.scripts[scriptName] = symbols.scripts[scriptMapping[item]];
            symbols.scripts.erase(scriptMapping[item]);
            scriptMapping[item] = scriptName;
        }
    }
    connectUI();
}

void MainWindow::slot_textEdit_scripts_textChanged() {
    std::string text = ui->textEdit_scripts->toPlainText().toStdString();
    if (currentScript != nullptr) {
        symbols.scripts[scriptMapping[currentScript]].body = text;
    }
}

void MainWindow::slotFunctionArgCountChange(int args) {
    ui->lineEdit_functions_arg0->setVisible(false);
    ui->lineEdit_functions_arg1->setVisible(false);
    ui->lineEdit_functions_arg2->setVisible(false);
    ui->lineEdit_functions_arg3->setVisible(false);
    ui->lineEdit_functions_arg4->setVisible(false);

    ui->lineEdit_functions_arg0->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
    ui->lineEdit_functions_arg1->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
    ui->lineEdit_functions_arg2->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
    ui->lineEdit_functions_arg3->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
    ui->lineEdit_functions_arg4->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);

    if (currentFunction == nullptr || currentFunction->row() == 0) {
        ui->spinBox_functions_argcount->setValue(0);
        return;
    }

    switch (args) {
        case 5:
            ui->lineEdit_functions_arg4->setVisible(true);
            ui->lineEdit_functions_arg4->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        case 4:
            ui->lineEdit_functions_arg3->setVisible(true);
            ui->lineEdit_functions_arg3->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        case 3:
            ui->lineEdit_functions_arg2->setVisible(true);
            ui->lineEdit_functions_arg2->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        case 2:
            ui->lineEdit_functions_arg1->setVisible(true);
            ui->lineEdit_functions_arg1->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        case 1:
            ui->lineEdit_functions_arg0->setVisible(true);
            ui->lineEdit_functions_arg0->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        case 0:
            break;
        default:
            throw std::runtime_error("Invalid valid received from function arg count");
    }
    if (currentFunction != nullptr) {
        symbols.functions[functionMapping[currentFunction]].argumentNames.resize(args);
    }
}

void MainWindow::slotFunctionArgChange0(const QString &arg) {
    if (currentFunction != nullptr && !arg.isEmpty()) {
        Function &fun = symbols.functions[functionMapping[currentFunction]];
        if (fun.argumentNames.size() >= 1) {
            fun.argumentNames[0] = arg.toStdString();
        } else {
            fun.argumentNames.emplace_back(arg.toStdString());
        }
    }
}

void MainWindow::slotFunctionArgChange1(const QString &arg) {
    if (currentFunction != nullptr && !arg.isEmpty()) {
        Function &fun = symbols.functions[functionMapping[currentFunction]];
        if (fun.argumentNames.size() >= 2) {
            fun.argumentNames[1] = arg.toStdString();
        } else {
            fun.argumentNames.resize(2);
            fun.argumentNames[1] = arg.toStdString();
        }
    }
}

void MainWindow::slotFunctionArgChange2(const QString &arg) {
    if (currentFunction != nullptr && !arg.isEmpty()) {
        Function &fun = symbols.functions[functionMapping[currentFunction]];
        if (fun.argumentNames.size() >= 3) {
            fun.argumentNames[2] = arg.toStdString();
        } else {
            fun.argumentNames.resize(3);
            fun.argumentNames[2] = arg.toStdString();
        }
    }
}

void MainWindow::slotFunctionArgChange3(const QString &arg) {
    if (currentFunction != nullptr && !arg.isEmpty()) {
        Function &fun = symbols.functions[functionMapping[currentFunction]];
        if (fun.argumentNames.size() >= 4) {
            fun.argumentNames[3] = arg.toStdString();
        } else {
            fun.argumentNames.resize(4);
            fun.argumentNames[3] = arg.toStdString();
        }
    }
}

void MainWindow::slotFunctionArgChange4(const QString &arg) {
    if (currentFunction != nullptr && !arg.isEmpty()) {
        Function &fun = symbols.functions[functionMapping[currentFunction]];
        if (fun.argumentNames.size() >= 5) {
            fun.argumentNames[4] = arg.toStdString();
        } else {
            fun.argumentNames.resize(5);
            fun.argumentNames[4] = arg.toStdString();
        }
    }
}

void MainWindow::slotFunctionBodyChange() {
    std::string text = ui->textEdit_functions_body->toPlainText().toStdString();
    if (currentFunction != nullptr) {
        symbols.functions[functionMapping[currentFunction]].expression = text;
    }
}

void MainWindow::slotFunctionTableWidgetCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    currentFunction = current;
    disconnectUI();
    ui->textEdit_functions_body->setText("");
    connectUI();
    ui->spinBox_functions_argcount->setValue(0);
    if (current == nullptr) {
        ui->textEdit_functions_body->setEnabled(false);
        ui->spinBox_functions_argcount->setEnabled(false);
    } else if (current->row() != 0) {
        ui->textEdit_functions_body->setEnabled(true);
        ui->spinBox_functions_argcount->setEnabled(true);

        ui->textEdit_functions_body->setText(symbols.functions[functionMapping[current]].expression.c_str());

        std::vector<std::string> args = symbols.functions[functionMapping[current]].argumentNames;
        ui->spinBox_functions_argcount->setValue(args.size());

        ui->lineEdit_functions_arg0->setText("");
        ui->lineEdit_functions_arg0->hide();
        ui->lineEdit_functions_arg1->setText("");
        ui->lineEdit_functions_arg1->hide();
        ui->lineEdit_functions_arg2->setText("");
        ui->lineEdit_functions_arg2->hide();
        ui->lineEdit_functions_arg3->setText("");
        ui->lineEdit_functions_arg3->hide();
        ui->lineEdit_functions_arg4->setText("");
        ui->lineEdit_functions_arg4->hide();

        switch (args.size()) {
            case 5:
                ui->lineEdit_functions_arg4->show();
                ui->lineEdit_functions_arg4->setText(args[4].c_str());
            case 4:
                ui->lineEdit_functions_arg3->show();
                ui->lineEdit_functions_arg3->setText(args[3].c_str());
            case 3:
                ui->lineEdit_functions_arg2->show();
                ui->lineEdit_functions_arg2->setText(args[2].c_str());
            case 2:
                ui->lineEdit_functions_arg1->show();
                ui->lineEdit_functions_arg1->setText(args[1].c_str());
            case 1:
                ui->lineEdit_functions_arg0->show();
                ui->lineEdit_functions_arg0->setText(args[0].c_str());
                break;
        }
    } else {
        //Default entry
        ui->textEdit_functions_body->setEnabled(false);
        ui->spinBox_functions_argcount->setEnabled(false);
    }
}

void MainWindow::slotFunctionTableWidgetItemChanged(QTableWidgetItem *item) {
    if (item == nullptr)
        return;

    disconnectUI();

    std::string functionName = item->text().toStdString();
    if (item->row() == 0) {
        //Create
        currentFunction = item;
        if (functionName.empty()) {
            QMessageBox::warning(this, "Error", "Function name cannot be empty.");
        } else if (symbols.functions.find(functionName) != symbols.functions.end()) {
            QMessageBox::warning(this, "Error",
                                 "Function with name " + QString(functionName.c_str()) + " already exists.");
        } else {
            functionMapping[item] = functionName;
            symbols.functions[functionName] = Function();
            symbols.functions[functionName].name = functionName;
            ui->tableWidget_functions->insertRow(0);
            ui->tableWidget_functions->setCurrentItem(
                    ui->tableWidget_functions->item(1, 0)
            );
            ui->textEdit_functions_body->setEnabled(true);
            ui->spinBox_functions_argcount->setEnabled(true);
        }
    } else {
        //Update name
        if (functionName.empty()) {
            QMessageBox::warning(this, "Error", "Function name cannot be empty.");
        } else if (functionName != functionMapping[item]
                   && symbols.functions.find(functionName) != symbols.functions.end()) {
            QMessageBox::warning(this, "Error",
                                 "Function with name " + QString(functionName.c_str()) + " already exists.");
            item->setText(functionMapping[item].c_str());
        } else {
            symbols.functions[functionName] = symbols.functions[functionMapping[item]];
            symbols.functions[functionName].name = functionName;
            symbols.functions.erase(functionMapping[item]);
            functionMapping[item] = functionName;
        }
    }

    connectUI();
}

void MainWindow::slotVariablesTableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    disconnectUI();

    if (current == nullptr) {
    } else if (current->row() != 0) {
    } else {
        //Default entry
    }

    connectUI();
}

void MainWindow::slotVariablesTableWidget_itemChanged(QTableWidgetItem *item) {
    if (item == nullptr)
        return;

    QTableWidgetItem *nameItem = ui->tableWidget_variables->item(item->row(), 0);
    std::string itemName;
    if (nameItem != nullptr)
        itemName = nameItem->text().toStdString();

    QTableWidgetItem *valueItem = ui->tableWidget_variables->item(item->row(), 1);
    std::string itemValue;
    if (valueItem != nullptr)
        itemValue = valueItem->text().toStdString();

    if (item->row() == 0) {
        //Create
        if (itemName.empty()) {
            QMessageBox::warning(this, "Error", "Variable name cannot be empty.");
        } else if (symbols.variables.find(itemName) != symbols.variables.end()) {
            QMessageBox::warning(this, "Error",
                                 "Variable with name " + QString(itemName.c_str()) + " already exists.");
        } else {
            variableMapping[nameItem] = itemName;

            try {
                symbols.variables[itemName] = std::stold(itemValue);
            }
            catch (std::exception &e) {
                symbols.variables[itemName] = 0;
            }

            ui->tableWidget_variables->insertRow(0);
            ui->tableWidget_variables->setCurrentItem(nameItem);
        }
    } else {
        //Update name and value
        if (itemName.empty()) {
            QMessageBox::warning(this, "Error", "Variable name cannot be empty.");
        } else if (itemName != variableMapping[nameItem]
                   && (symbols.variables.find(itemName) != symbols.variables.end())) {
            QMessageBox::warning(this, "Error",
                                 "Variable with name " + QString(itemName.c_str()) + " already exists.");
        } else {
            symbols.variables[itemName] = symbols.variables[variableMapping[nameItem]];
            symbols.variables.erase(variableMapping[nameItem]);
            variableMapping[nameItem] = itemName;
        }
        try {
            symbols.variables[variableMapping[nameItem]] = std::stold(itemValue);
        }
        catch (std::exception &e) {
            symbols.variables[variableMapping[nameItem]] = 0;
        }
    }
    if (nameItem != nullptr)
        nameItem->setText(variableMapping[nameItem].c_str());
    if (valueItem != nullptr)
        valueItem->setText(std::to_string(symbols.variables[variableMapping[nameItem]]).c_str());
}

void MainWindow::slotConstantsTableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    disconnectUI();

    if (current == nullptr) {
    } else if (current->row() != 0) {
    } else {
        //Default entry
    }

    connectUI();
}

void MainWindow::slotConstantsTableWidget_itemChanged(QTableWidgetItem *item) {
    if (item == nullptr)
        return;

    QTableWidgetItem *nameItem = ui->tableWidget_constants->item(item->row(), 0);
    std::string itemName;
    if (nameItem != nullptr)
        itemName = nameItem->text().toStdString();

    QTableWidgetItem *valueItem = ui->tableWidget_constants->item(item->row(), 1);
    std::string itemValue;
    if (valueItem != nullptr)
        itemValue = valueItem->text().toStdString();

    if (item->row() == 0) {
        //Create
        if (itemName.empty()) {
            QMessageBox::warning(this, "Error", "Constant name cannot be empty.");
        } else if (symbols.constants.find(itemName) != symbols.constants.end()) {
            QMessageBox::warning(this, "Error",
                                 "Constant with name " + QString(itemName.c_str()) + " already exists.");
        } else {
            constantsMapping[nameItem] = itemName;
            try {
                symbols.constants[itemName] = std::stold(itemValue);
            }
            catch (std::exception &e) {
                symbols.constants[itemName] = 0;
            }

            ui->tableWidget_constants->insertRow(0);
            ui->tableWidget_constants->setCurrentItem(nameItem);
        }
    } else {
        //Update name and value
        if (itemName.empty()) {
            QMessageBox::warning(this, "Error", "Constant name cannot be empty.");
        } else if (itemName != constantsMapping[nameItem]
                   && (symbols.constants.find(itemName) != symbols.constants.end())) {
            QMessageBox::warning(this, "Error",
                                 "Constant with name " + QString(itemName.c_str()) + " already exists.");
        } else {
            symbols.constants[itemName] = symbols.constants[constantsMapping[nameItem]];
            symbols.constants.erase(constantsMapping[nameItem]);
            constantsMapping[nameItem] = itemName;
        }
        try {
            symbols.constants[constantsMapping[nameItem]] = std::stold(itemValue);
        }
        catch (std::exception &e) {
            symbols.constants[constantsMapping[nameItem]] = 0;
        }
    }
    if (nameItem != nullptr)
        nameItem->setText(constantsMapping[nameItem].c_str());
    if (valueItem != nullptr)
        valueItem->setText(std::to_string(symbols.constants[constantsMapping[nameItem]]).c_str());
}
