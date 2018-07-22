#include "vidext.h"
#include "glwindow.h"
#include "emucontroller.h"
#include "../error.h"
#include "../common.h"

#include <m64p_types.h>
#include <QSurfaceFormat>
#include <QCoreApplication>

#define FROM "vidext"

GlWindow *glWindow;
static QSurfaceFormat format;
extern EmuController emulation;

static m64p_error init()
{
    LOG(L_VERB, FROM, "init");
    format = QSurfaceFormat();
    format.setMajorVersion(2);
    format.setMinorVersion(1);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setDepthBufferSize(24);
    return M64ERR_SUCCESS;
}

static m64p_error quit()
{
    LOG(L_VERB, FROM, "quit");
}

static m64p_error listModes(m64p_2d_size *sizes, int *nSizes)
{
    LOG(L_VERB, FROM, "listModes");
}

static m64p_error setMode(int width, int height, int, int mode, int)
{
    LOG(L_VERB, FROM, "setMode");
    emulation.createGlWindow(&format);
    emulation.resize(width, height);
    glWindow->makeCurrent();
}

static void *glGetProc(const char *proc)
{
    LOG(L_VERB, FROM, "glGetProc");
}

static m64p_error glSetAttr(m64p_GLattr attr, int value)
{
    printf("%d\n", value);
    switch (attr) {
    case M64P_GL_DOUBLEBUFFER:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_DOUBLEBUFFER");
        if (value == 1) {
            format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        } else if (value == 0) {
            format.setSwapBehavior(QSurfaceFormat::SingleBuffer);
        }
        return M64ERR_SUCCESS;
    case M64P_GL_BUFFER_SIZE:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_BUFFER_SIZE");
        return M64ERR_SUCCESS;
    case M64P_GL_DEPTH_SIZE:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_DEPTH_SIZE");
        format.setDepthBufferSize(value);
        return M64ERR_SUCCESS;
    case M64P_GL_RED_SIZE:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_RED_SIZE");
        format.setRedBufferSize(value);
        return M64ERR_SUCCESS;
    case M64P_GL_GREEN_SIZE:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_GREEN_SIZE");
        format.setGreenBufferSize(value);
        return M64ERR_SUCCESS;
    case M64P_GL_BLUE_SIZE:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_BLUE_SIZE");
        format.setBlueBufferSize(value);
        return M64ERR_SUCCESS;
    case M64P_GL_ALPHA_SIZE:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_ALPHA_SIZE");
        format.setAlphaBufferSize(value);
        return M64ERR_SUCCESS;
    case M64P_GL_SWAP_CONTROL:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_SWAP_CONTROL");
        format.setSwapInterval(value);
        return M64ERR_SUCCESS;
    case M64P_GL_MULTISAMPLEBUFFERS:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_MULTISAMPLEBUFFERS");
        return M64ERR_SUCCESS;
    case M64P_GL_MULTISAMPLESAMPLES:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_MULTISAMPLESAMPLES");
        format.setSamples(value);
        return M64ERR_SUCCESS;
    case M64P_GL_CONTEXT_MAJOR_VERSION:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_CONTEXT_MAJOR_VERSION");
        format.setMajorVersion(value);
        return M64ERR_SUCCESS;
    case M64P_GL_CONTEXT_MINOR_VERSION:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_CONTEXT_MINOR_VERSION");
        format.setMinorVersion(value);
        return M64ERR_SUCCESS;
    case M64P_GL_CONTEXT_PROFILE_MASK:
        LOG(L_VERB, FROM, "glSetAttr: M64P_GL_CONTEXT_PROFILE_MASK");
        switch (value) {
        case M64P_GL_CONTEXT_PROFILE_CORE:
            format.setProfile(QSurfaceFormat::CoreProfile);
            break;
        case M64P_GL_CONTEXT_PROFILE_COMPATIBILITY:
            format.setProfile(QSurfaceFormat::CompatibilityProfile);
            break;
        case M64P_GL_CONTEXT_PROFILE_ES:
            format.setRenderableType(QSurfaceFormat::OpenGLES);
            break;
        default:
            LOG(L_WARN, FROM, TR("glSetAttr: invalid profile value: ")
                    + QString::number(value));
            return M64ERR_INPUT_INVALID;
        }
        return M64ERR_SUCCESS;
    }
    LOG(L_WARN, FROM, TR("glSetAttr: invalid attr: ")
            + QString::number(attr));
    return M64ERR_INPUT_INVALID;
}

static m64p_error glGetAttr(m64p_GLattr attr, int *value)
{
    LOG(L_VERB, FROM, "glGetAttr");
}

static m64p_error glSwapBuf()
{
    //LOG(L_VERB, FROM, "glSwapBuf");
    glWindow->context()->swapBuffers(glWindow);
    return M64ERR_SUCCESS;
}

static m64p_error setCaption(const char *title)
{
    LOG(L_VERB, FROM, "setCaption");
}

static m64p_error toggleFs()
{
    LOG(L_VERB, FROM, "toggleFs");
}

static m64p_error resizeWindow(int, int)
{
    LOG(L_VERB, FROM, "resizeWindow");
}

static uint32_t glGetDefaultFb()
{
    LOG(L_VERB, FROM, "glGetDefaultFb");
}


m64p_video_extension_functions vidextFunctions = {
    12,
    init,       quit,      listModes,    setMode,
    glGetProc,  glSetAttr, glGetAttr,    glSwapBuf,
    setCaption, toggleFs,  resizeWindow, glGetDefaultFb,
};
