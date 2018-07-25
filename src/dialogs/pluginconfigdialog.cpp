#include "pluginconfigdialog.h"
#include "../core.h"
#include "../error.h"
#include "../common.h"

#include <QGridLayout>
#include <QScrollArea>
#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QAbstractButton>

#include <string>


static QString toSectionName(const QString &name)
{
    return QString(name).remove("mupen64plus-");
}


struct ConfParam
{
    QString name;
    m64p_type type;
};


static void receiveParameter(void *listp, const char *name, m64p_type type)
{
    std::vector<ConfParam> *list = (std::vector<ConfParam> *)listp;
    list->push_back({name, type});
}


PluginConfigDialog::PluginConfigDialog(const QString &name, QWidget *parent)
    : QDialog(parent)
{
    sectionName = toSectionName(name);
    m64p_handle configHandle;
    m64p_error rval;

    rval = ConfigOpenSection(sectionName.toUtf8().data(), &configHandle);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not open section <Section>").replace("<Section>", name));
        return;
    }

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QScrollArea *scrollArea = new QScrollArea();
    QGridLayout *gridLayout = new QGridLayout();
    QWidget *gridContainer = new QWidget();
    gridContainer->setLayout(gridLayout);
    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);

    std::vector<ConfParam> list;
    rval = ConfigListParameters(configHandle, &list, receiveParameter);
    int col1row = 0;
    int col2row = 0;
    for (size_t i = 0; i < list.size(); i++) {
        ConfParam &p = list[i];
        QByteArray name_ba = p.name.toUtf8();
        const char *name_cp = name_ba.data();
        QString name = p.name;
        QString help_string = ConfigGetParameterHelp(configHandle, name_cp);
        QString help = "<p>" + help_string.replace(": ", ":</p><p>") + "</p>";

        switch (p.type) {
        case M64TYPE_INT:
            {
                int value = ConfigGetParamInt(configHandle, name_cp);
                QLabel *label = new QLabel(name);
                label->setToolTip(help);
                gridLayout->addWidget(label, col1row, 0, Qt::AlignRight);
                QSpinBox *input = new QSpinBox();
                input->setMinimum(-99999);
                input->setMaximum(99999);
                input->setValue(value);
                input->setToolTip(help);
                gridLayout->addWidget(input, col1row, 1, Qt::AlignLeft);
                ints.push_back({configHandle, name_ba, input});
            }
            col1row++;
            break;
        case M64TYPE_FLOAT:
            LOG_W("Missed float " + name);
            break;
        case M64TYPE_BOOL:
            {
                bool value = ConfigGetParamBool(configHandle, name_cp);
                QCheckBox *cb = new QCheckBox(name);
                cb->setToolTip(help);
                cb->setCheckState(value ? Qt::Checked : Qt::Unchecked);
                gridLayout->addWidget(cb, col2row, 2, Qt::AlignLeft);
                bools.push_back({configHandle, name_ba, cb});
            }
            col2row++;
            break;
        case M64TYPE_STRING:
            {
                const char *value = ConfigGetParamString(configHandle, name_cp);
                QLabel *label = new QLabel(name);
                label->setToolTip(help);
                gridLayout->addWidget(label, col1row, 0, Qt::AlignRight);
                QLineEdit *input = new QLineEdit();
                input->setToolTip(help);
                input->setText(value);
                gridLayout->addWidget(input, col1row, 1, Qt::AlignLeft);
                strings.push_back({configHandle, name_ba, input});
            }
            col1row++;
            break;
        }
    }

    layout->addWidget(scrollArea);

    QDialogButtonBox *buttons = new QDialogButtonBox();
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(layout);
    resize(700, 500);
}


void PluginConfigDialog::accept()
{
    for (size_t i = 0; i < ints.size(); i++) {
        ConfItem<QSpinBox> &o = ints[i];
        int value = o.value->value();
        ConfigSetParameter(o.handle, o.name.data(), M64TYPE_INT, &value);
    }
    for (size_t i = 0; i < bools.size(); i++) {
        ConfItem<QCheckBox> &o = bools[i];
        int value = o.value->isChecked();
        ConfigSetParameter(o.handle, o.name.data(), M64TYPE_BOOL, &value);
    }
    for (size_t i = 0; i < strings.size(); i++) {
        ConfItem<QLineEdit> &o = strings[i];
        QByteArray value_ba = o.value->text().toUtf8();
        const char *value = value_ba.data();
        ConfigSetParameter(o.handle, o.name.data(), M64TYPE_STRING, value);
    }

    m64p_error rval;
    rval = ConfigSaveSection(sectionName.toUtf8().data());
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not save configuration: ") + m64errstr(rval));
    }

    close();
}
