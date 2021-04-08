#ifndef QCALC_FUNCTIONSEDITOR_HPP
#define QCALC_FUNCTIONSEDITOR_HPP

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "calc/function.hpp"

class FunctionsEditor : public QWidget {
Q_OBJECT
public:
    explicit FunctionsEditor(QWidget *parent = nullptr);

public slots:

    void setFunctions(const std::map<std::string, Function> &functions);

    void setCurrentFunction(const QString &name);

signals:

    void onFunctionAdded(const QString &name);

    void onFunctionNameChanged(const QString &originalName, const QString &name);

    void onFunctionBodyChanged(const QString &name, const QString &body);

    void onFunctionArgsChanged(const QString &name, const std::vector<std::string> &args);

    void onCurrentFunctionChanged(const QString &name);

private slots:

    void onFunctionAddPressed();

    void onFunctionArgEditingFinished();

    void onFunctionArgsSpinBoxChanged(int value);

    void onTableCellActivated(int row, int column);

    void onTableCellChanged(int row ,int column);

    void onFunctionExpressionChanged();

private:
    void applyArgs(const std::vector<std::string> &args);

    std::map<std::string, Function> functions;

    std::map<std::string, int> rowMapping;

    std::string currentFunction;

    QTableWidget *list;

    QLineEdit *addLineEdit;
    QPushButton *addPushButton;

    QSpinBox *argsSpinBox;
    QLineEdit *argEdit0;
    QLineEdit *argEdit1;
    QLineEdit *argEdit2;
    QLineEdit *argEdit3;
    QLineEdit *argEdit4;

    QTextEdit *expressionEdit;
};

#endif //QCALC_FUNCTIONSEDITOR_HPP
