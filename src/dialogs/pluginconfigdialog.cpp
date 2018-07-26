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
#include <QComboBox>
#include <QRegularExpression>

#include <string>


static QString camelSpaced(const QString &s)
{
    QString ret;
    bool lastUpper = false;
    for (int i = 0; i < s.length(); i++) {
        bool isLast = i == s.length() - 1;
        bool isUpper = s[i].isUpper();
        bool nextUpper = !isLast && s[i+1].isUpper();
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
        .split(". ")[0]
        .trimmed();
    if (firstPart.length() > 0 && firstPart[firstPart.length() - 1] == '.') {
        firstPart.chop(1);
    }

    if (firstPart.length() < 35 && firstPart.length() > niceName.length()) {
        return firstPart;
    } else {
        return niceName;
    }
}


struct IntOption
{
    int number;
    QString desc;
};

static std::vector<IntOption> helpToOptions(const QString &help)
{
    QRegularExpression re("((?:[<>]=?)?[\\d,-]+)\\s?=\\s?([\\w/ %:-]+)");
    std::vector<IntOption> ret;
    auto matches = re.globalMatch(help);
    while (matches.hasNext()) {
        auto m = matches.next();
        if (m.captured(1).indexOf('-') > 0
                || m.captured(1).contains(',')
                || m.captured(1).contains('<')
                || m.captured(1).contains('>')) {
            return {};
        }
        int n = m.captured(1).toInt();
        QString s = m.captured(2) + " [" + QString::number(n) + "]";
        ret.push_back({n, s});
    }
    return ret;
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
        QString help_html = "<p>[" + name + "]</p>"
            + "<p>"
            + QString(help_string).replace(": ", ":</p><p>")
            + "</p>";
        QString desc = toReadableName(name, help_string);

        switch (p.type) {
        case M64TYPE_INT:
            {
                int value = ConfigGetParamInt(configHandle, name_cp);
                QLabel *label = new QLabel(desc);
                label->setToolTip(help_html);
                gridLayout->addWidget(label, col1row, 0, Qt::AlignRight);
                std::vector<IntOption> options = helpToOptions(help_string);
                if (options.empty()) {
                    QSpinBox *input = new QSpinBox();
                    input->setMinimum(-99999);
                    input->setMaximum(99999);
                    input->setValue(value);
                    input->setToolTip(help_html);
                    gridLayout->addWidget(input, col1row, 1, Qt::AlignLeft);
                    ints.push_back({configHandle, name, input, label, help_string});
                } else {
                    QComboBox *input = new QComboBox();
                    input->setMinimumContentsLength(12);
                    int selectedIndex = -1;
                    for (size_t i = 0; i < options.size(); i++) {
                        IntOption &o = options[i];
                        input->addItem(o.desc, o.number);
                        if (o.number == value) {
                            selectedIndex = i;
                        }
                    }
                    input->setCurrentIndex(selectedIndex);
                    input->setToolTip(help_html);
                    gridLayout->addWidget(input, col1row, 1, Qt::AlignLeft);
                    combos.push_back({configHandle, name, input, label, help_string});
                }
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
                cb->setToolTip(help_html);
                cb->setCheckState(value ? Qt::Checked : Qt::Unchecked);
                gridLayout->addWidget(cb, col2row, 2, Qt::AlignLeft);
                bools.push_back({configHandle, name, cb, NULL, help_string});
            }
            col2row++;
            break;
        case M64TYPE_STRING:
            {
                const char *value = ConfigGetParamString(configHandle, name_cp);
                QLabel *label = new QLabel(desc);
                label->setToolTip(help_html);
                gridLayout->addWidget(label, col1row, 0, Qt::AlignRight);
                QLineEdit *input = new QLineEdit();
                input->setToolTip(help_html);
                input->setText(value);
                gridLayout->addWidget(input, col1row, 1, Qt::AlignLeft);
                strings.push_back({configHandle, name, input, label, help_string});
            }
            col1row++;
            break;
        }
    }

    layout->addWidget(scrollArea);

    QLineEdit *search = new QLineEdit();
    connect(search, SIGNAL(textEdited(const QString &)),
            this, SLOT(search(const QString &)));

    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    boxLayout->addWidget(new QLabel("Search:"));
    boxLayout->addWidget(search);
    layout->addLayout(boxLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox();
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    boxLayout->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(layout);
    resize(740, 500);
}


void PluginConfigDialog::accept()
{
    for (auto &o : ints) {
        int value = o.value->value();
        ConfigSetParameter(o.handle, o.name.toUtf8().data(), M64TYPE_INT, &value);
    }
    for (auto &o : combos) {
        int value = o.value->currentData().toInt();
        ConfigSetParameter(o.handle, o.name.toUtf8().data(), M64TYPE_INT, &value);
    }
    for (auto &o : bools) {
        int value = o.value->isChecked();
        ConfigSetParameter(o.handle, o.name.toUtf8().data(), M64TYPE_BOOL, &value);
    }
    for (auto &o : strings) {
        QByteArray value_ba = o.value->text().toUtf8();
        const char *value = value_ba.data();
        ConfigSetParameter(o.handle, o.name.toUtf8().data(), M64TYPE_STRING, value);
    }

    m64p_error rval;
    rval = ConfigSaveSection(sectionName.toUtf8().data());
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not save configuration: ") + m64errstr(rval));
    }

    close();
}


static bool matches(const QString &text, const QString &name, const QString &help)
{
    return text.isEmpty()
        || help.contains(text, Qt::CaseInsensitive)
        || name.contains(text, Qt::CaseInsensitive);
}


void PluginConfigDialog::search(const QString &text)
{
    for (auto &o : ints) {
        bool m = matches(text, o.name, o.help);
        o.value->setVisible(m);
        if (o.label) {
            o.label->setVisible(m);
        }
    }
    for (auto &o : combos) {
        bool m = matches(text, o.name, o.help);
        o.value->setVisible(m);
        if (o.label) {
            o.label->setVisible(m);
        }
    }
    for (auto &o : bools) {
        bool m = matches(text, o.name, o.help);
        o.value->setVisible(m);
        if (o.label) {
            o.label->setVisible(m);
        }
    }
    for (auto &o : strings) {
        bool m = matches(text, o.name, o.help);
        o.value->setVisible(m);
        if (o.label) {
            o.label->setVisible(m);
        }
    }
}
