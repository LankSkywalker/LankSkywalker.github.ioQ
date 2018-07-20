#include "core.h"
#include "common.h"
#include "error.h"
#include "osal/osal_dynamiclib.h"
#include "osal/osal_preproc.h"

#include <cassert>
#include <QObject>
#include <QMessageBox>

#define MINIMUM_CORE_VERSION   0x016300
#define OUR_CORE_API_VERSION   0x020001
#define OUR_CONFIG_API_VERSION 0x020000

Core *Core::instance = NULL;

static bool api_versions_compatible(int v1, int v2)
{
    return (v1 & 0xffff0000) == (v2 & 0xffff0000);
}

void debug_callback(void *context, int level, const char *message)
{
    const char *context_str = (const char *)context;
    enum log_level l = level_from_m64((m64p_msg_level)level);
    if (l <= L_WARN) {
        LOG(l, context_str, message);
    }
}

void core_state_callback(void *context, m64p_core_param, int)
{
    const char *context_str = (const char *)context;
    LOG(L_INFO, context_str, "STATE CALLBACK");
}

Core::Core()
{
}

bool Core::init()
{
    assert(instance == NULL);
    instance = this;

    m64p_error rval;

    rval = osal_dynlib_open(&libhandle, OSAL_DEFAULT_DYNLIB_FILENAME);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not open core library: ") + m64errstr(rval));
        return false;
    }

    m64p_plugin_type core_plugin_type;
    int core_version;
    int core_api_version;
    const char *core_name;
    int core_capabilities;
    rval = PluginGetVersion(&core_plugin_type, &core_version,
            &core_api_version, &core_name, &core_capabilities);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not get core version info: ") + m64errstr(rval));
        return false;
    }
    printf("%s v. %x, api v. %x, type %d, cap %d\n",
            core_name, core_version, core_api_version,
            core_plugin_type, core_capabilities);
    if (core_plugin_type != M64PLUGIN_CORE) {
        SHOW_E(TR("The core library is not the core library."));
        return false;
    }
    if (core_version < MINIMUM_CORE_VERSION) {
        SHOW_E(TR("The core library is too old."));
        return false;
    }
    if (!api_versions_compatible(core_api_version, OUR_CORE_API_VERSION)) {
        SHOW_E(TR("The core has incompatible API version."));
        return false;
    }

    int c = core_capabilities;
    LOG_I(TR("Core capabilities:"));
    LOG_I(TR("  [<X>] Dynamic recompiler")
            .replace("<X>", c & M64CAPS_DYNAREC ? "x" : " "));
    LOG_I(TR("  [<X>] Debugger")
            .replace("<X>", c & M64CAPS_DEBUGGER ? "x" : " "));
    LOG_I(TR("  [<X>] Core comparison")
            .replace("<X>", c & M64CAPS_CORE_COMPARE ? "x" : " "));

    int config_version, debug_version, vidext_version;
    rval = CoreGetAPIVersions(&config_version, &debug_version, &vidext_version, NULL);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not get core API versions: ") + m64errstr(rval));
        return false;
    }
    if (!api_versions_compatible(config_version, OUR_CONFIG_API_VERSION)) {
        SHOW_E(TR("The core has incompatible config version."));
        return false;
    }

    const int frontend_api_version = 0x020102;
    const char *config_dir = NULL;
    const char *data_dir = NULL;
    static char core_id[] = "Core";
    rval = CoreStartup(frontend_api_version, config_dir, data_dir,
            core_id, debug_callback, NULL, core_state_callback);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not initialize core: ") + m64errstr(rval));
        return false;
    }

    return true;
}

Core &Core::get()
{
    assert(instance != NULL);
    return *instance;
}

m64p_dynlib_handle Core::get_libhandle() const
{
    return libhandle;
}

Core::~Core()
{
    osal_dynlib_close(libhandle);
    CoreShutdown();
}
