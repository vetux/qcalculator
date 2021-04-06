#ifndef QCALC_ADDONITEMWIDGET_HPP
#define QCALC_ADDONITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

class AddonItemWidget : public QWidget {
Q_OBJECT
public:
    explicit AddonItemWidget(QWidget *parent = nullptr);

    void setModuleName(const QString &name);

    QString getModuleName();

    void setModuleEnabled(bool enabled);

    bool getModuleEnabled();

    void setModuleDisplayName(const QString &name);

    void setModuleDescription(const QString &description);

signals:

    void onModuleEnabledChanged(bool enabled);

private:
    QString moduleName;
    QHBoxLayout *layout;
    QCheckBox *checkbox;
    QLabel *label;
    QPushButton *button;

private slots:

    void onCheckBoxStateChange(int state);

    void onButtonPressed();
};

#endif //QCALC_ADDONITEMWIDGET_HPP
