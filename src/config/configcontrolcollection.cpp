/***
 * Copyright (c) 2018, Robert Alm Nilsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the organization nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***/

#include "configcontrolcollection.h"
#include "../core.h"
#include "../error.h"

#include <QStringList>
#include <QRegularExpression>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>


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

    if (firstPart.length() < 32 && firstPart.length() > niceName.length()) {
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

    // Fix options that have flags (1, 2, 4...) but no 0.
    if (!ret.empty()) {
        bool noProblem = false;
        int expected = 1;
        for (auto &o : ret) {
            if (o.number == 0) {
                noProblem = true;
                break;
            }
            if (o.number != expected) {
                noProblem = true;
                break;
            }
            expected *= 2;
        }
        if (!noProblem) {
            ret.insert(ret.begin(), {0, QString("None [0]")});
        }
    }

    return ret;
}


ConfigControlCollection::ConfigControlCollection()
    : configHandle(NULL)
{
}


void ConfigControlCollection::addItem(m64p_type type, const char *name)
{
    assert(configHandle);
    ConfItem item(type, name, configHandle);
    items.push_back(item);
}


std::vector<ConfItem> &ConfigControlCollection::getItems()
{
    return items;
}


bool ConfigControlCollection::removeByConfigName(const char *configName)
{
    for (size_t i = 0; i < items.size(); i++) {
        const ConfItem &item = items[i];
        if (item.name.compare(configName, Qt::CaseInsensitive) == 0) {
            items.erase(items.begin() + i);
            return true;
        }
    }
    return false;
}


void ConfigControlCollection::save() const
{
    for (const ConfItem &item : items) {
        QByteArray nameBa = item.name.toUtf8();
        const char *name = nameBa.data();
        switch (item.type) {
        case M64TYPE_INT:
            {
                int value;
                QComboBox *w = dynamic_cast<QComboBox*>(item.widget);
                if (w) {
                    value = w->currentData().toInt();
                } else {
                    QSpinBox *w = dynamic_cast<QSpinBox*>(item.widget);
                    value = w->value();
                }
                ConfigSetParameter(configHandle, name, M64TYPE_INT, &value);
            }
            break;
        case M64TYPE_FLOAT:
            // TODO: float
            break;
        case M64TYPE_BOOL:
            {
                QCheckBox *w = dynamic_cast<QCheckBox*>(item.widget);
                int value = w->isChecked();
                ConfigSetParameter(configHandle, name, M64TYPE_BOOL, &value);
            }
            break;
        case M64TYPE_STRING:
            {
                QLineEdit *w = dynamic_cast<QLineEdit*>(item.widget);
                QByteArray valueBa = w->text().toUtf8();
                const char *value = valueBa.data();
                ConfigSetParameter(configHandle, name, M64TYPE_STRING, value);
            }
            break;
        }
    }
}


static bool matches(const QString &text, const QString &name, const QString &help)
{
    return text.isEmpty()
        || help.contains(text, Qt::CaseInsensitive)
        || name.contains(text, Qt::CaseInsensitive);
}


void ConfigControlCollection::filter(const QString &text)
{
    for (ConfItem &item : items) {
        bool m = matches(text, item.name, item.help);
        item.widget->setVisible(m);
        if (item.label) {
            item.label->setVisible(m);
        }
    }
}


void ConfigControlCollection::setConfigHandle(m64p_handle configHandle)
{
    this->configHandle = configHandle;
}


void ConfItem::createWidget()
{
    QByteArray name_ba = name.toUtf8();
    const char *name_cp = name_ba.data();
    QString help_string = ConfigGetParameterHelp(configHandle, name_cp);
    QString help_html = "<p>[" + name + "]</p>"
        + "<p>"
        + QString(help_string).replace(": ", ":</p><p>")
        + "</p>";
    QString desc = toReadableName(name, help_string);

    switch (type) {
    case M64TYPE_INT:
        {
            int value = ConfigGetParamInt(configHandle, name_cp);
            QLabel *label = new QLabel(desc);
            label->setToolTip(help_html);
            this->label = label;
            std::vector<IntOption> options = helpToOptions(help_string);
            if (options.empty()) {
                QSpinBox *input = new QSpinBox();
                input->setMinimum(-99999);
                input->setMaximum(99999);
                input->setValue(value);
                input->setToolTip(help_html);
                this->widget = input;
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
                this->widget = input;
            }
        }
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
            this->widget = cb;
        }
        break;
    case M64TYPE_STRING:
        {
            const char *value = ConfigGetParamString(configHandle, name_cp);
            QLabel *label = new QLabel(desc);
            label->setToolTip(help_html);
            this->label = label;
            QLineEdit *input = new QLineEdit();
            input->setToolTip(help_html);
            input->setText(value);
            this->widget = input;
        }
        break;
    }
}
