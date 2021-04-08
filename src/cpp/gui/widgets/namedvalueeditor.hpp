#ifndef QCALC_NAMEDVALUEEDITOR_HPP
#define QCALC_NAMEDVALUEEDITOR_HPP

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

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

    void onNameChanged(const QString &name);

    void onValueChanged(const QString &value);

private:
    QListWidget *list;
    QLineEdit *addLineEditName;
    QLineEdit *addLineEditValue;
    QPushButton *addPushButton;
};

#endif //QCALC_NAMEDVALUEEDITOR_HPP
