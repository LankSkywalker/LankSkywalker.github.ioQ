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


static QString camelSpaced(const QString &s)
{
    QString ret;
    bool lastUpper = false;
    for (int i = 0; i < s.length(); i++) {
        bool isLast = i == s.length() - 1;
        bool isUpper = s[i] >= 'A' && s[i] <= 'Z';
        bool nextUpper = !isLast && s[i+1] >= 'A' && s[i+1] <= 'Z';
        if (isUpper && (!lastUpper || !nextUpper) && i > 0 && !isLast) {
            ret += " ";
        }
        if (i == 0) {
            ret += s[i].toUpper();
        } else if (isUpper && (nextUpper || (isLast && lastUpper))) {
            ret += s[i];
        } else {
            ret += s[i].toLower();
        }
        lastUpper = isUpper;
    }
    return ret;
}

static QString toReadableName(QString name, QString help)
{
    QString niceName;
    if (name.toUpper() == name) {
        name = name.toLower();
    }
    name[0] = name[0].toUpper();
    QStringList nameWords = name.split("_");
    if (nameWords.length() == 1) {
        niceName = camelSpaced(name);
    } else {
        niceName = nameWords.join(" ");
    }
    QString firstPart = help
        .split(":")[0]
        .split(" -- ")[0]
        .split(" (")[0]
        .split(". ")[0];
    if (firstPart.length() > 0 && firstPart[firstPart.length() - 1] == '.') {
        firstPart.truncate(firstPart.length() - 1);
    }

    if (firstPart.length() < 35 && firstPart.length() > niceName.length()) {
        return firstPart;
    } else {
        return niceName;
    }
}


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
        QString help = "<p>[" + name + "]</p>"
            + "<p>"
            + QString(help_string).replace(": ", ":</p><p>")
            + "</p>";
        QString desc = toReadableName(name, help_string);

        switch (p.type) {
        case M64TYPE_INT:
            {
                int value = ConfigGetParamInt(configHandle, name_cp);
                QLabel *label = new QLabel(desc);
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
                QCheckBox *cb = new QCheckBox(desc);
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
                QLabel *label = new QLabel(desc);
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
