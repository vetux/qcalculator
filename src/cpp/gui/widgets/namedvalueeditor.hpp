#ifndef QCALC_NAMEDVALUEEDITOR_HPP
#define QCALC_NAMEDVALUEEDITOR_HPP

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>

#include <map>

class NamedValueEditor : public QWidget {
Q_OBJECT
public:
    explicit NamedValueEditor(QWidget *parent = nullptr);

public slots:

    void setValues(const std::map<QString, QString> &values);

signals:

    void onNamedValueAdded(const QString &name, const QString &value);

    void onNameChanged(const QString &originalName, const QString &name);

    void onValueChanged(const QString &name, const QString &value);

private slots:
    void onAddPressed();

    void onTableCellChanged(int row, int column);

private:
    QTableWidget *list;
    QLineEdit *addLineEditName;
    QLineEdit *addLineEditValue;
    QPushButton *addPushButton;

    std::map<int, std::string> mapping;
};

#endif //QCALC_NAMEDVALUEEDITOR_HPP
