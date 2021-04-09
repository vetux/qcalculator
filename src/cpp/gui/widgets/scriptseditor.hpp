#ifndef QCALC_SCRIPTSEDITOR_HPP
#define QCALC_SCRIPTSEDITOR_HPP

#include <QWidget>

#include "math/script.hpp"

class ScriptsEditor : public QWidget {
Q_OBJECT
public:
    explicit ScriptsEditor(QWidget *parent = nullptr);

public slots:

    void setScripts(const std::map<std::string, Script> &scripts);
};

#endif //QCALC_SCRIPTSEDITOR_HPP
