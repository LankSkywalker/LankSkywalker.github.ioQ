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

#include "emulation.h"
#include "emuthread.h"
#include "../core.h"
#include "../plugin.h"
#include "../global.h"
#include "../error.h"
#include "../common.h"
#include "../osal/osal_dynamiclib.h"

#include <m64p_types.h>

extern Emulation emulation;
EmuThread *emuthread = NULL;


static m64p_dynlib_handle pluginRsp, pluginGfx, pluginAudio, pluginInput;

static bool runRom(void *romData, int length);
static bool attachPlugin(m64p_plugin_type type,
        m64p_dynlib_handle &plugin, const QString &name, char *typestr);
static bool attachPlugins();
static void detachPlugins();


void Emulation::startGame(const QString &romFileName, const QString &zipFileName)
{
    emuthread = new EmuThread(romFileName, zipFileName);
    emuthread->start();
}


void Emulation::runGame(const QString &romFileName, const QString &zipFileName)
{
    QByteArray romData;
    readRomFile(romData, romFileName, zipFileName);

    if (romData.isEmpty()) {
        SHOW_W(TR("Could not read ROM file."));
        return;
    }

    if (!romData.startsWith("\x80\x37\x12\x40")
            && !romData.startsWith("\x37\x80\x40\x12")) {
        SHOW_W(TR("Not a valid ROM File."));
        return;
    }

    emit started();

    runRom(romData.data(), romData.length());

    emit finished();
}


static bool runRom(void *romData, int length)
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_ROM_OPEN, length, romData);
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not load the ROM: ") + m64errstr(rval));
        return false;
    }

    if (!attachPlugins()) {
        return false;
    }

    // This is where the game actually runs.
    rval = CoreDoCommand(M64CMD_EXECUTE, 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not start the ROM: ") + m64errstr(rval));
        CoreDoCommand(M64CMD_ROM_CLOSE, 0, NULL);
        detachPlugins();
        return false;
    }

    rval = CoreDoCommand(M64CMD_ROM_CLOSE, 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not close the ROM: ") + m64errstr(rval));
        return false;
    }
    detachPlugins();

    return true;
}


static bool attachPlugin(m64p_plugin_type type,
        m64p_dynlib_handle &plugin, const QString &name, char *typestr)
{
    LOG_I(TR("Starting <Type> plugin...").replace("<Type>", typestr));
    m64p_error rval;
    bool ok;
    ok = openPlugin(plugin, name.toUtf8().data(), typestr);
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


static bool attachPlugins()
{
    QString name;
    name = SETTINGS.value("Plugins/video", "").toString();
    if (!attachPlugin(M64PLUGIN_GFX, pluginGfx, name, (char *)"video")) {
        return false;
    }
    name = SETTINGS.value("Plugins/audio", "").toString();
    if (!attachPlugin(M64PLUGIN_AUDIO, pluginAudio, name, (char *)"audio")) {
        return false;
    }
    name = SETTINGS.value("Plugins/input", "").toString();
    if (!attachPlugin(M64PLUGIN_INPUT, pluginInput, name, (char *)"input")) {
        return false;
    }
    name = SETTINGS.value("Plugins/rsp", "").toString();
    if (!attachPlugin(M64PLUGIN_RSP, pluginRsp, name, (char *)"RSP")) {
        return false;
    }

    return true;
}


static void detachPlugins()
{
    CoreDetachPlugin(M64PLUGIN_RSP);
    closePlugin(pluginRsp);
    CoreDetachPlugin(M64PLUGIN_INPUT);
    closePlugin(pluginInput);
    CoreDetachPlugin(M64PLUGIN_AUDIO);
    closePlugin(pluginAudio);
    CoreDetachPlugin(M64PLUGIN_GFX);
    closePlugin(pluginGfx);
}


bool Emulation::isExecuting()
{
    m64p_error rval;
    int state;
    rval = CoreDoCommand(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &state);
    return state != M64EMU_STOPPED;
}


void Emulation::stopGame()
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_STOP, 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        SHOW_W(TR("Could not stop the game: ") + m64errstr(rval));
        return;
    }
}


void Emulation::play()
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_RESUME, 0, NULL);
}


void Emulation::pause()
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_PAUSE, 0, NULL);
}


void Emulation::advanceFrame()
{
    CoreDoCommand(M64CMD_ADVANCE_FRAME, 0, NULL);
}


void Emulation::saveState()
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_STATE_SAVE, 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        LOG_W(TR("Could not save state: ") + m64errstr(rval));
    }
}


void Emulation::loadState()
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_STATE_LOAD, 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        LOG_W(TR("Could not load state: ") + m64errstr(rval));
    }
}


void Emulation::setSaveSlot(int n)
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_STATE_SET_SLOT, n, NULL);
    if (rval != M64ERR_SUCCESS) {
        LOG_W(TR("Could not set save slot: ") + m64errstr(rval));
    }
}


void Emulation::reset(bool hard)
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_RESET, hard ? 1 : 0, NULL);
    if (rval != M64ERR_SUCCESS) {
        LOG_W(TR("Could not reset: ") + m64errstr(rval));
    }
}


void Emulation::resetSoft()
{
    reset(false);
}


void Emulation::resetHard()
{
    reset(true);
}


bool Emulation::getRomSettings(size_t size, m64p_rom_settings *romSettings)
{
    m64p_error rval;
    rval = CoreDoCommand(M64CMD_ROM_GET_SETTINGS, size, romSettings);
    if (rval != M64ERR_SUCCESS) {
        LOG_W(TR("Could not get ROM settings: ") + m64errstr(rval));
    }
}
