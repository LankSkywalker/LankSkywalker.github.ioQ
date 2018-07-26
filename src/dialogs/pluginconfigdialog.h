#ifndef PLUGINCONFIGDIALOG_H
#define PLUGINCONFIGDIALOG_H

#include <m64p_types.h>

#include <vector>
#include <QDialog>
#include <QLabel>
class QAbstractButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLineEdit;


template <typename V>
struct ConfItem
{
    m64p_handle handle;
    QString name;
    V *value;
    QLabel *label;
    QString help;
};


class PluginConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginConfigDialog(const QString &name, QWidget *parent = NULL);

private slots:
    void accept();
    void search(const QString &text);

private:
    QString sectionName;
    std::vector<ConfItem<QSpinBox>> ints;
    std::vector<ConfItem<QComboBox>> combos;
    std::vector<ConfItem<QCheckBox>> bools;
    std::vector<ConfItem<QLineEdit>> strings;
};


#endif // PLUGINCONFIGDIALOG_H
