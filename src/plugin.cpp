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

#include "plugin.h"
#include "global.h"
#include "common.h"
#include "core.h"
#include "error.h"
#include "osal/osal_dynamiclib.h"

#include <QString>

#ifdef Q_OS_WIN
#define FILENAME_EXTENSION ".dll"
#else
#define FILENAME_EXTENSION ".so"
#endif

#include <m64p_common.h>

static QString findPlugin(const char *name)
{
    QString dir = SETTINGS.value("Paths/plugins", "").toString();
    return dir + "/" + name + FILENAME_EXTENSION;
}

bool openPlugin(m64p_dynlib_handle &plugin, const char *name, char *type)
{
    QByteArray qfilename = findPlugin(name).toUtf8();
    const char *filename = qfilename.data();
    m64p_error rval;

    rval = osal_dynlib_open(&plugin, filename);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not open plugin <PluginName>.")
                   .replace("<PluginName>", filename) + ": " + m64errstr(rval));
        return false;
    }

    ptr_PluginStartup pluginStartup
        = (ptr_PluginStartup)osal_dynlib_getproc(plugin, "PluginStartup");
    if (pluginStartup == NULL) {
        SHOW_E(TR("Plugin <PluginName> is broken, PluginStartup not found.")
                .replace("<PluginName>", filename));
        return false;
    }

    void debugCallback(void*, int, const char*);
    rval = pluginStartup(Core::get().getLibhandle(), type, debugCallback);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Plugin <PluginName> could not be started: ")
                .replace("<PluginName>", filename) + m64errstr(rval));
        return false;
    }

    return true;
}

bool closePlugin(m64p_dynlib_handle &lib)
{
    m64p_error rval;
    ptr_PluginShutdown pluginShutdown
        = (ptr_PluginShutdown)osal_dynlib_getproc(lib, "PluginShutdown");
    if (pluginShutdown == NULL) {
        SHOW_W(TR("Could not shut down plugin (function not found)."));
    } else {
        rval = pluginShutdown();
        if (rval != M64ERR_SUCCESS) {
            SHOW_W(TR("Could not shut down plugin."));
            return false;
        }
    }

    osal_dynlib_close(lib);

    return true;
}
