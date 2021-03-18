#include "mainwindow.hpp"

#include <QScrollBar>
#include <QMessageBox>
#include <QTextStream>
#include <QStandardPaths>

#include "ui_mainwindow.h"

#include "numberformat.hpp"
#include "fractiontest.hpp"

#define OBJECT_NAME_PREFIX_BITS "pushButton_bit_"
#define OBJECT_NAME_PREFIX_KEYPAD "pushButton_kp_"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          ui(new Ui::MainWindow),
          presenter(model, *this),
          model(std::bind(&Presenter::onStateChange, &presenter, std::placeholders::_1)) {
    ui->setupUi(this);
    presenter.init();
}

MainWindow::~MainWindow() {
    delete ui;
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

void MainWindow::closeEvent(QCloseEvent *event) {
    presenter.onWindowClose(event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    presenter.onWindowResize(event);
}

void MainWindow::connectPresenter(const Presenter &target) {
    connect(ui->lineEdit_input, SIGNAL(returnPressed()), &target, SLOT(onInputSubmit()));
    connect(ui->lineEdit_input, SIGNAL(textChanged(const QString &)), &target, SLOT(onInputUpdate(const QString &)));
    connect(ui->actionSettings, SIGNAL(triggered(bool)), &target, SLOT(onActionSettings()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), &target, SLOT(onActionExit()));
    connect(ui->actionAbout_Qt_Calculator, SIGNAL(triggered(bool)), &target, SLOT(onActionAbout()));
    connect(ui->actionShow_Keypad, SIGNAL(toggled(bool)), &target, SLOT(onActionShowKeyPad(bool)));
    connect(ui->actionShow_Bit_Toggle, SIGNAL(toggled(bool)), &target, SLOT(onActionShowBitView(bool)));
    connect(ui->actionShow_Dock, SIGNAL(toggled(bool)), &target, SLOT(onActionShowDock(bool)));
}

void MainWindow::disconnectPresenter(const Presenter &target) {
    disconnect(ui->lineEdit_input, SIGNAL(returnPressed()), &target, SLOT(onInputSubmit()));
    disconnect(ui->lineEdit_input, SIGNAL(textChanged(const QString &)), &target, SLOT(onInputUpdate(const QString &)));
    disconnect(ui->actionSettings, SIGNAL(triggered(bool)), &target, SLOT(onActionSettings()));
    disconnect(ui->actionExit, SIGNAL(triggered(bool)), &target, SLOT(onActionExit()));
    disconnect(ui->actionAbout_Qt_Calculator, SIGNAL(triggered(bool)), &target, SLOT(onActionAbout()));
    disconnect(ui->actionShow_Keypad, SIGNAL(toggled(bool)), &target, SLOT(onActionShowKeyPad(bool)));
    disconnect(ui->actionShow_Bit_Toggle, SIGNAL(toggled(bool)), &target, SLOT(onActionShowBitView(bool)));
    disconnect(ui->actionShow_Dock, SIGNAL(toggled(bool)), &target, SLOT(onActionShowDock(bool)));
}

void MainWindow::launchAboutDialog() {

}

void MainWindow::launchSettingsDialog() {

}

void MainWindow::quit() {
    exit(0);
}

void MainWindow::setInputText(const std::string &value) {
    ui->lineEdit_input->setText(value.c_str());
}

void MainWindow::setKeyPadVisibility(bool visible) {
    ui->widget_keypad->setVisible(visible);
}

void MainWindow::setBitViewVisibility(bool visible) {
    ui->widget_bits->setVisible(visible);
}

void MainWindow::setDockVisibility(bool visible) {
    ui->dockWidget->setVisible(visible);
}

void MainWindow::setActiveDockTab(int tab) {
    ui->tabWidget_2->setCurrentIndex(tab);
}

void MainWindow::setBitViewContents(std::bitset<64> value) {

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
    ui->textEdit_history->setText("HISTORY SET");
}

void MainWindow::setVariableListView(const std::vector<std::pair<std::string, std::string>> &value) {

}

void MainWindow::setActiveVariable(int index) {

}

void MainWindow::setConstantsListView(const std::vector<std::pair<std::string, std::string>> &value) {

}

void MainWindow::setActiveConstant(int index) {

}

void MainWindow::setFunctionsListView(const std::vector<std::string> &value) {

}

void MainWindow::setActiveFunction(int index) {

}

void MainWindow::setFunctionArgs(const std::vector<std::string> &value) {

}

void MainWindow::setFunctionBody(const std::string &value) {

}

void MainWindow::setFunctionArgsEnabled(bool enabled) {

}

void MainWindow::setFunctionBodyEnabled(bool enabled) {

}

void MainWindow::setScriptsListView(const std::vector<std::string> &value) {

}

void MainWindow::setActiveScript(int index) {

}

void MainWindow::setScriptBody(const std::string &value) {

}

void MainWindow::setScriptBodyEnabled(bool enabled) {

}

void MainWindow::setWindowSize(QSize size) {
    resize(size);
}
