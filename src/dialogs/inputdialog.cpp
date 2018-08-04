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

#include "inputdialog.h"
#include "ui_inputdialog.h"
#include "../common.h"
#include "../error.h"
#include "../core.h"
#include "../plugin.h"


static QString toSectionName(const QString &name, int controllerNumber)
{
    return QString(name).remove("mupen64plus-")
        + "-control" + QString::number(controllerNumber);
}


static bool openSection(const QString &sectionName, m64p_handle &configHandle)
{
    m64p_error rval;
    rval = ConfigOpenSection(sectionName.toUtf8().data(), &configHandle);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not open section <Section>")
                .replace("<Section>", sectionName));
        return false;
    }
    return true;
}


InputDialog::InputDialog(const QString &name, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InputDialog)
    , pluginName(name)
{
    ui->setupUi(this);

    loadUnloadPlugin(name.toUtf8().data());

    for (int i = 1; i <= 4; i++) {
        m64p_handle configHandle;
        QString sectionName = toSectionName(name, i);
        openSection(sectionName, configHandle);
        controllers.push_back({sectionName, configHandle, {}});
        ui->controllerBox->addItem(TR("Controller <N>").replace("<N>", QString::number(i)));
    }
    connect(ui->controllerBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(controllerSelected(int)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    getButtons();
    setValues();
}


InputDialog::~InputDialog()
{
    delete ui;
}


void InputDialog::getButtons()
{
    buttons = {
        {"X Axis",           ui->leftButton},
        {"Y Axis",           ui->upButton},
        {"A Button",         ui->aButton},
        {"B Button",         ui->bButton},
        {"C Button U",       ui->cUpButton},
        {"C Button D",       ui->cDownButton},
        {"C Button L",       ui->cLeftButton},
        {"C Button R",       ui->cRightButton},
        {"DPad U",           ui->dUpButton},
        {"DPad D",           ui->dDownButton},
        {"DPad L",           ui->dLeftButton},
        {"DPad R",           ui->dRightButton},
        {"Mempak switch",    ui->memButton},
        {"Rumblepak switch", ui->rumbleButton},
    };
}



static std::vector<KeySpec> parseKeyConfig(const char *str)
{
    std::vector<KeySpec> ret;
    for (;;) {
        KeySpec k;
        if (!KeySpec::parseOne(k, &str)) {
            break;
        }
        ret.push_back(k);
    }
    return ret;
}


static QString keyspecsToString(const std::vector<KeySpec> &keyspecs)
{
    QString str;
    bool first = true;
    for (const KeySpec &k : keyspecs) {
        if (!first) {
            str += " ";
        }
        str += k.toString();
        first = false;
    }
    return str;
}


void InputDialog::setValues()
{
    Controller &c = controllers[currentController];

    // First get the values from core if we don't have them already.
    if (c.values.empty()) {
        for (const Button &k : buttons) {
            const char *value = ConfigGetParamString(c.configHandle, k.configName);
            std::vector<KeySpec> keyspecs = parseKeyConfig(value);
            c.values.push_back(Value{k.configName, keyspecs});
        }
    }

    // And then show them on the buttons.
    for (Button &k : buttons) {
        std::vector<KeySpec> keyspecs;
        // Find the value for this button.
        for (const Value &v : c.values) {
            if (strcmp(v.configName, k.configName) == 0) {
                keyspecs = v.keys;
                break;
            }
        }
        if (keyspecs.empty()) {
            k.button->setText("Select...");
        } else {
            k.button->setText(keyspecsToString(keyspecs));
        }
    }
}


void InputDialog::saveController(int controllerIndex)
{
    const Controller &c = controllers[currentController];
    m64p_error rval;

    for (const Value &v : c.values) {
        QString valueStr = keyspecsToString(v.keys);
        QByteArray valueBa = valueStr.toUtf8();
        const char *value = valueBa.data();;
        rval = ConfigSetParameter(c.configHandle, v.configName,
                M64TYPE_STRING, value);
        if (rval != M64ERR_SUCCESS) {
            LOG_W(TR("Could not set configuration parameter <Name>: ")
                    .replace("<Name>", v.configName) + m64errstr(rval));
        }
    }

    QString sectionName = toSectionName(pluginName, controllerIndex + 1);
    rval = ConfigSaveSection(sectionName.toUtf8().data());
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not save configuration: ") + m64errstr(rval));
    }
}


void InputDialog::accept()
{
    for (int i = 0; i < 4; i++) {
        saveController(i);
    }
    close();
}


void InputDialog::controllerSelected(int index)
{
    currentController = index;
    setValues();
}


void InputDialog::loadUnloadPlugin(const char *name)
{
    m64p_dynlib_handle h;
    if (openPlugin(h, name, "input")) {
        closePlugin(h);
    }
}
