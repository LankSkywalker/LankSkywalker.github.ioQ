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

static QString find_plugin(const char *name)
{
    QString dir = SETTINGS.value("Paths/plugins", "").toString();
    return dir + "/" + name + FILENAME_EXTENSION;
}

bool open_plugin(m64p_dynlib_handle &plugin, const char *name, char *type)
{
    QByteArray qfilename = find_plugin(name).toUtf8();
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

    void debug_callback(void*, int, const char*);
    rval = pluginStartup(Core::get().get_libhandle(), type, debug_callback);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Plugin <PluginName> could not be started: ")
                .replace("<PluginName>", filename) + m64errstr(rval));
        return false;
    }

    return true;
}

bool close_plugin(m64p_dynlib_handle &lib)
{
}
