#ifndef QCALC_FUNCTIONITEMWIDGET_HPP
#define QCALC_FUNCTIONITEMWIDGET_HPP

#include <QLineEdit>

class FunctionItemWidget : public QLineEdit {
Q_OBJECT
public:
    explicit FunctionItemWidget(QWidget *parent) : QLineEdit(parent) {}

    QString getName() { return name; }

public slots:

    void setName(const QString &text) {
        name = text;
        setText(text);
    };

private:
    QString name;
};

#endif //QCALC_FUNCTIONITEMWIDGET_HPP
