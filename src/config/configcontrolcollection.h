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

#ifndef CONFIGCONTROLCOLLECTION_H
#define CONFIGCONTROLCOLLECTION_H

#include <m64p_types.h>
#include <vector>
#include <QString>
class QWidget;
class QLabel;


struct ConfItem
{
    m64p_type type;
    QString name;
    QWidget *widget;
    QLabel *label;
    QString help;
    m64p_handle configHandle;

    ConfItem(m64p_type type, const QString &name, m64p_handle configHandle)
        : type(type)
        , name(name)
        , widget(NULL)
        , label(NULL)
        , help("")
        , configHandle(configHandle)
    {
        createWidget();
    }

private:
    void createWidget();
};


class ConfigControlCollection
{
public:
    ConfigControlCollection();
    void addItem(m64p_type type, const char *name);
    std::vector<ConfItem> &getItems();
    void save() const;
    void filter(const QString &text);
    void setConfigHandle(m64p_handle configHandle);

private:
    m64p_handle configHandle;
    std::vector<ConfItem> items;
};


#endif // CONFIGCONTROLCOLLECTION_H
