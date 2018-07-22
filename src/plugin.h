#ifndef PLUGIN_H
#define PLUGIN_H

#include <m64p_types.h>

bool openPlugin(m64p_dynlib_handle &handle, const char *name, char *type);

bool closePlugin(m64p_dynlib_handle &lib);

#endif // PLUGIN_H
