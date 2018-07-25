#include "core.h"
#include "common.h"
#include "error.h"
#include "emulation/vidext.h"
#include "emulation/emucontroller.h"
#include "osal/osal_dynamiclib.h"
#include "osal/osal_preproc.h"

#include <cassert>
#include <QObject>
#include <QMessageBox>

#define MINIMUM_CORE_VERSION   0x016300
#define OUR_CORE_API_VERSION   0x020001
#define OUR_CONFIG_API_VERSION 0x020000

Core *Core::instance = NULL;

static bool apiVersionsCompatible(int v1, int v2)
{
    return (v1 & 0xffff0000) == (v2 & 0xffff0000);
}

void debugCallback(void *context, int level, const char *message)
{
    const char *contextStr = (const char *)context;
    LogLevel l = levelFromM64((m64p_msg_level)level);
    LOG(l, contextStr, message);
}

void coreStateCallback(void *context, m64p_core_param param, int value)
{
    extern EmuController emulation;
    const char *contextStr = (const char *)context;
    if (param == M64CORE_EMU_STATE) {
        if (value == M64EMU_RUNNING) {
            emulation.emitResumed();
        } else if (value == M64EMU_PAUSED) {
            emulation.emitPaused();
        }
    }
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

    m64p_plugin_type corePluginType;
    int coreVersion;
    int coreApiVersion;
    const char *coreName;
    int coreCapabilities;
    rval = PluginGetVersion(&corePluginType, &coreVersion,
            &coreApiVersion, &coreName, &coreCapabilities);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not get core version info: ") + m64errstr(rval));
        return false;
    }
    printf("%s v. %x, api v. %x, type %d, cap %d\n",
            coreName, coreVersion, coreApiVersion,
            corePluginType, coreCapabilities);
    if (corePluginType != M64PLUGIN_CORE) {
        SHOW_E(TR("The core library is not the core library."));
        return false;
    }
    if (coreVersion < MINIMUM_CORE_VERSION) {
        SHOW_E(TR("The core library is too old."));
        return false;
    }
    if (!apiVersionsCompatible(coreApiVersion, OUR_CORE_API_VERSION)) {
        SHOW_E(TR("The core has incompatible API version."));
        return false;
    }

    int c = coreCapabilities;
    LOG_I(TR("Core capabilities:"));
    LOG_I(TR("  [<X>] Dynamic recompiler")
            .replace("<X>", c & M64CAPS_DYNAREC ? "x" : " "));
    LOG_I(TR("  [<X>] Debugger")
            .replace("<X>", c & M64CAPS_DEBUGGER ? "x" : " "));
    LOG_I(TR("  [<X>] Core comparison")
            .replace("<X>", c & M64CAPS_CORE_COMPARE ? "x" : " "));

    int configVersion, debugVersion, vidextVersion;
    rval = CoreGetAPIVersions(&configVersion, &debugVersion, &vidextVersion, NULL);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not get core API versions: ") + m64errstr(rval));
        return false;
    }
    if (!apiVersionsCompatible(configVersion, OUR_CONFIG_API_VERSION)) {
        SHOW_E(TR("The core has incompatible config version."));
        return false;
    }

    const int frontendApiVersion = 0x020102;
    const char *configDir = NULL;
    const char *dataDir = NULL;
    static char coreId[] = "Core";
    rval = CoreStartup(frontendApiVersion, configDir, dataDir,
            coreId, debugCallback, NULL, coreStateCallback);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not initialize core: ") + m64errstr(rval));
        return false;
    }

    rval = CoreOverrideVidExt(&vidextFunctions);
    if (rval != M64ERR_SUCCESS) {
        SHOW_E(TR("Could not override video extensions: ") + m64errstr(rval));
        return false;
    }

    return true;
}

Core &Core::get()
{
    assert(instance != NULL);
    return *instance;
}

m64p_dynlib_handle Core::getLibhandle() const
{
    return libhandle;
}

Core::~Core()
{
    osal_dynlib_close(libhandle);
    CoreShutdown();
}
