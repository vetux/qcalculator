#ifndef QCALC_HISTORYITEMWIDGET_HPP
#define QCALC_HISTORYITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class HistoryItemWidget : public QWidget {
Q_OBJECT
public:
    explicit HistoryItemWidget(QWidget *parent = nullptr) : QWidget(parent) {
        layout = new QHBoxLayout(this);

        equalsText = new QLabel(this);

        expressionText = new QLabel(this);
        resultText = new QLabel(this);

        equalsText->setTextInteractionFlags(Qt::TextSelectableByMouse);

        expressionText->setTextInteractionFlags(Qt::TextSelectableByMouse);
        resultText->setTextInteractionFlags(Qt::TextSelectableByMouse);

        equalsText->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        equalsText->setAlignment(Qt::AlignCenter);

        resultText->setAlignment(Qt::AlignRight);
        resultText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        expressionText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        layout->addWidget(expressionText, 6);
        layout->addWidget(equalsText, 1);
        layout->addWidget(resultText, 3);

        equalsText->setText("=");
    }

    void resizeEvent(QResizeEvent *event) override {
        updateText();
    }

    void setContents(const std::string &e, const std::string &r) {
        expression = e.c_str();
        result = r.c_str();
        updateText();
    }

    void updateText() {
        expressionText->setText("");
        resultText->setText("");
        layout->activate();

        QFontMetrics metrics = QFontMetrics(expressionText->font());

        QString elitext = metrics.elidedText(expression, Qt::ElideRight, expressionText->width());
        expressionText->setText(elitext);
        if (expression != elitext)
            expressionText->setToolTip(expression);
        else
            expressionText->setToolTip("");

        elitext = metrics.elidedText(result, Qt::ElideRight, resultText->width());
        resultText->setText(elitext);
        if (result != elitext)
            resultText->setToolTip(result);
        else
            resultText->setToolTip("");
    }

private:
    QString expression;
    QString result;
    QHBoxLayout *layout;
    QLabel *equalsText;
    QLabel *expressionText;
    QLabel *resultText;
};

#endif //QCALC_HISTORYITEMWIDGET_HPP
