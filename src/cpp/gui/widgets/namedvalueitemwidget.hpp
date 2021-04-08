#ifndef QCALC_NAMEDVALUEITEMWIDGET_HPP
#define QCALC_NAMEDVALUEITEMWIDGET_HPP

#include <QWidget>
#include <QLineEdit>

class NamedValueItemWidget : public QWidget {
Q_OBJECT
public:
    explicit NamedValueItemWidget(QWidget *parent = nullptr);

    QString getName();

    QString getValue();

signals:

    void onNameChanged(const QString &name);

    void onValueChanged(const QString &value);

public slots:

    void setName(const QString &name);

    void setValue(const QString &value);

private slots:

    void onNameEditingFinished();

    void onValueEditingFinished();

private:
    QString nameText;
    QString valueText;

    QLineEdit *nameEdit;
    QLineEdit *valueEdit;
};

#endif //QCALC_NAMEDVALUEITEMWIDGET_HPP
