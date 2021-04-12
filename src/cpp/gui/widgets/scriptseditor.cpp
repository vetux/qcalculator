#include "gui/widgets/scriptseditor.hpp"

#include <QVBoxLayout>
#include <QLabel>

class ScriptItemWidget : public QWidget {
public:
    explicit ScriptItemWidget(QWidget *parent) : QWidget(parent) {
        setLayout(new QHBoxLayout);
        label = new QLabel(this);
        layout()->addWidget(label);
    }

    void setText(const QString &text) {
        label->setText(text);
    }

private:
    QLabel *label;
};

ScriptsEditor::ScriptsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());
    list = new QListWidget(this);
    layout()->addWidget(list);
}

void ScriptsEditor::setScripts(const std::map<std::string, Script> &scripts) {
    list->clear();
    for (auto &p : scripts) {
        auto *widget = new ScriptItemWidget(list);
        auto *item = new QListWidgetItem();

        QString text = p.first.c_str();

        if (p.second.enableArguments)
            text.append(" - Arguments enabled");

        widget->setText(text);

        item->setSizeHint(widget->sizeHint());

        list->addItem(item);
        list->setItemWidget(item, widget);

        list->update();
    }
}
