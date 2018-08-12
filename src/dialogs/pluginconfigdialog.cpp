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

#include "pluginconfigdialog.h"
#include "../core.h"
#include "../error.h"
#include "../common.h"
#include "../global.h"
#include "../plugin.h"

#include <QGridLayout>
#include <QScrollArea>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>


static QString toSectionName(const QString &name)
{
    return QString(name).remove("mupen64plus-");
}


static void receiveParameter(void *data, const char *name, m64p_type type)
{
    auto *configs = (ConfigControlCollection *)data;
    configs->addItem(type, name);
}


PluginConfigDialog::PluginConfigDialog(const QString &name, QWidget *parent)
    : QDialog(parent)
{
    loadUnloadPlugin(name.toUtf8().data());

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

    configs.setConfigHandle(configHandle);
    rval = ConfigListParameters(configHandle, &configs, receiveParameter);
    int col1row = 0;
    int col2row = 0;
    for (const ConfItem &confItem : configs.getItems()) {
        QWidget *widget = confItem.widget;
        if (confItem.type == M64TYPE_BOOL) {
            gridLayout->addWidget(widget, col2row, 2, Qt::AlignLeft);
            col2row++;
        } else {
            gridLayout->addWidget(confItem.label, col1row, 0, Qt::AlignRight);
            gridLayout->addWidget(widget, col1row, 1, Qt::AlignLeft);
            col1row++;
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
    setWindowTitle(name);
}


void PluginConfigDialog::accept()
{
    configs.save();

    m64p_error rval;
    rval = ConfigSaveSection(sectionName.toUtf8().data());
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not save configuration: ") + m64errstr(rval));
    }

    close();
}


void PluginConfigDialog::search(const QString &text)
{
    configs.filter(text);
}


static QString nameToType(const QString &name)
{
    return name.section('-', 1, 1);
}


void PluginConfigDialog::loadUnloadPlugin(const char *name)
{
    m64p_dynlib_handle h;
    if (openPlugin(h, name, nameToType(name).toUtf8().data())) {
        closePlugin(h);
    }
}
