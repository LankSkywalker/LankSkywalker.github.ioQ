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

#include "settings.h"
#include "global.h"
#include "osal/osal_preproc.h"

#include <QDir>


static QDir getPluginPath()
{
    return QDir(SETTINGS.value("Paths/plugins", "").toString());
}


static QStringList getAvailablePluginsMatching(QString pattern)
{
    QStringList pats {pattern + OSAL_DLL_EXTENSION};
    QFileInfoList files = getPluginPath().entryInfoList(pats, QDir::Files);
    QStringList result;
    foreach(QFileInfo file, files) {
        result << file.baseName();
    }
    return result;
}


QStringList getAvailableVideoPlugins()
{
    return getAvailablePluginsMatching("mupen64plus-video-*");
}


QStringList getAvailableAudioPlugins()
{
    return getAvailablePluginsMatching("mupen64plus-audio-*");
}


QStringList getAvailableInputPlugins()
{
    return getAvailablePluginsMatching("mupen64plus-input-*");
}


QStringList getAvailableRspPlugins()
{
    return getAvailablePluginsMatching("mupen64plus-rsp-*");
}


QString getCurrentVideoPlugin()
{
    QString defaultName = "mupen64plus-video-glide64mk2";
    QString plugin = SETTINGS.value("Plugins/video", defaultName).toString();
    return plugin;
}


QString getCurrentAudioPlugin()
{
    QString plugin = SETTINGS.value("Plugins/audio", "").toString();
    return plugin;
}


QString getCurrentInputPlugin()
{
    QString plugin = SETTINGS.value("Plugins/input", "").toString();
    return plugin;
}


QString getCurrentRspPlugin()
{
    QString plugin = SETTINGS.value("Plugins/rsp", "").toString();
    return plugin;
}
