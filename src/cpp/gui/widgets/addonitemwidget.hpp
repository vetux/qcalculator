#ifndef QCALC_ADDONITEMWIDGET_HPP
#define QCALC_ADDONITEMWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>

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

private slots:

    void onCheckBoxStateChange(int state);
};

#endif //QCALC_ADDONITEMWIDGET_HPP
