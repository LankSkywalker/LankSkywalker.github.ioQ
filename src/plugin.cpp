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
