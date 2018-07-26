#ifndef PLUGINCONFIGDIALOG_H
#define PLUGINCONFIGDIALOG_H

#include <m64p_types.h>

#include <vector>
#include <QDialog>
class QAbstractButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLineEdit;


template <typename V>
struct ConfItem
{
    m64p_handle handle;
    const QByteArray name;
    const V *value;
};


class PluginConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginConfigDialog(const QString &name, QWidget *parent = NULL);

private slots:
    void accept();

private:
    QString sectionName;
    std::vector<ConfItem<QSpinBox>> ints;
    std::vector<ConfItem<QComboBox>> combos;
    std::vector<ConfItem<QCheckBox>> bools;
    std::vector<ConfItem<QLineEdit>> strings;
};


#endif // PLUGINCONFIGDIALOG_H
