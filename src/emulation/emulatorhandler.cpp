/***
 * Copyright (c) 2013, Dan Hasting
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

#include "emulatorhandler.h"
#include "../plugin.h"
#include "../global.h"
#include "../common.h"
#include "../core.h"
#include "../error.h"

#include <QDir>
#include <QFile>
#include <QWidget>

#if QT_VERSION >= 0x050000
#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>
#else
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#endif


EmulatorHandler::EmulatorHandler(QWidget *parent)
    : QObject(parent)
{
    this->parent = parent;
}


void EmulatorHandler::emitFinished()
{
    emit finished();
}


void EmulatorHandler::startGame(QDir romDir, QString romFileName, QString zipFileName)
{
    QString completeRomPath;

    // If zipped file, extract and write to temp location for loading
    if (zipFileName != "") {
        QString zipFile = romDir.absoluteFilePath(zipFileName);
        QuaZipFile zippedFile(zipFile, romFileName);

        zippedFile.open(QIODevice::ReadOnly);
        QByteArray romData;
        romData.append(zippedFile.readAll());
        zippedFile.close();

        QString tempDir = QDir::tempPath() + "/" + AppNameLower + "/" + qgetenv("USER");
        QDir().mkpath(tempDir);
        completeRomPath = tempDir + "/temp.n64";

        QFile tempRom(completeRomPath);
        tempRom.open(QIODevice::WriteOnly);
        tempRom.write(romData);
        tempRom.close();
    } else {
        completeRomPath = romDir.absoluteFilePath(romFileName);
    }

    QFile romFile(completeRomPath);

    if (!romFile.exists() || QFileInfo(romFile).isDir()) {
        SHOW_W(TR("ROM file not found."));
        return;
    }

    romFile.open(QIODevice::ReadOnly);
    QByteArray contents = romFile.readAll();
    romFile.close();

    if (contents.isEmpty()) {
        SHOW_W(TR("Could not read ROM file."));
        return;
    }

    if (!contents.startsWith("\x80\x37\x12\x40")
            && !contents.startsWith("\x37\x80\x40\x12")) {
        SHOW_W(TR("Not a valid ROM File."));
        return;
    }

    emit started();

    if (!start_rom(contents.data(), contents.length())) {
        return;
    }

    emit finished();
}


bool EmulatorHandler::start_rom(void *romdata, int length)
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_ROM_OPEN, length, romdata);
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not load the ROM.") + m64errstr(rval));
        return false;
    }

    if (!attach_plugins()) {
        return false;
    }

    rval = CoreDoCommand(M64CMD_EXECUTE, 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        CoreDoCommand(M64CMD_ROM_CLOSE, 0, NULL);
        SHOW_W(TR("Could not start the ROM.") + m64errstr(rval));
        return false;
    }

    return true;
}


bool EmulatorHandler::attach_plugin(m64p_plugin_type type,
        m64p_dynlib_handle plugin, const QString &name, char *typestr)
{
    LOG_I(TR("Starting <Type> plugin...").replace("<Type>", typestr));
    m64p_error rval;
    bool ok;
    ok = open_plugin(plugin, name.toUtf8().data(), typestr);
    if (!ok) {
        SHOW_W(TR("Failed to open <Type> plugin.").replace("<Type>", typestr));
        return false;
    }

    rval = CoreAttachPlugin(type, plugin);
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Failed to attach <Type> plugin: ").replace("<Type>", typestr)
                + m64errstr(rval));
        return false;
    }
    return true;
}


bool EmulatorHandler::attach_plugins()
{
    QString name;
    name = SETTINGS.value("Plugins/video", "").toString();
    if (!attach_plugin(M64PLUGIN_GFX, plugin_gfx, name, (char *)"video")) {
        return false;
    }
    name = SETTINGS.value("Plugins/audio", "").toString();
    if (!attach_plugin(M64PLUGIN_AUDIO, plugin_audio, name, (char *)"audio")) {
        return false;
    }
    name = SETTINGS.value("Plugins/input", "").toString();
    if (!attach_plugin(M64PLUGIN_INPUT, plugin_input, name, (char *)"input")) {
        return false;
    }
    name = SETTINGS.value("Plugins/rsp", "").toString();
    if (!attach_plugin(M64PLUGIN_RSP, plugin_rsp, name, (char *)"RSP")) {
        return false;
    }

    return true;
}


bool EmulatorHandler::detach_plugins()
{
    CoreDetachPlugin(M64PLUGIN_RSP);
    close_plugin(plugin_rsp);
    CoreDetachPlugin(M64PLUGIN_INPUT);
    close_plugin(plugin_input);
    CoreDetachPlugin(M64PLUGIN_AUDIO);
    close_plugin(plugin_audio);
    CoreDetachPlugin(M64PLUGIN_GFX);
    close_plugin(plugin_gfx);
    return true;
}


void EmulatorHandler::stopGame()
{
    detach_plugins();
}
