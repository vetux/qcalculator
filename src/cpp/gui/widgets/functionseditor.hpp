#ifndef QCALC_FUNCTIONSEDITOR_HPP
#define QCALC_FUNCTIONSEDITOR_HPP

#include <QWidget>

#include "calc/function.hpp"

class FunctionsEditor : public QWidget {
Q_OBJECT
public:
    explicit FunctionsEditor(QWidget *parent = nullptr);

public slots:

    void setFunctions(const std::map<std::string, Function> &functions);

signals:

    void onFunctionsChanged(const std::map<std::string, Function> &functions);
};

#endif //QCALC_FUNCTIONSEDITOR_HPP
