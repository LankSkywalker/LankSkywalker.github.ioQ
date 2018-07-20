#ifndef PLUGIN_H
#define PLUGIN_H

#include <m64p_types.h>

bool open_plugin(m64p_dynlib_handle &handle, const char *name, char *type);

bool close_plugin(m64p_dynlib_handle &lib);

#endif // PLUGIN_H
