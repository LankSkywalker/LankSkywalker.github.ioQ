#include "error.h"

#include <QMessageBox>

const char *m64errstr(m64p_error error_value)
{
    switch (error_value) {
    case M64ERR_SUCCESS:         return "success";
    case M64ERR_NOT_INIT:        return "not_init";
    case M64ERR_ALREADY_INIT:    return "already_init";
    case M64ERR_INCOMPATIBLE:    return "incompatible";
    case M64ERR_INPUT_ASSERT:    return "input_assert";
    case M64ERR_INPUT_INVALID:   return "input_invalid";
    case M64ERR_INPUT_NOT_FOUND: return "input_not_found";
    case M64ERR_NO_MEMORY:       return "no_memory";
    case M64ERR_FILES:           return "files";
    case M64ERR_INTERNAL:        return "internal";
    case M64ERR_INVALID_STATE:   return "invalid_state";
    case M64ERR_PLUGIN_FAIL:     return "plugin_fail";
    case M64ERR_SYSTEM_FAIL:     return "system_fail";
    case M64ERR_UNSUPPORTED:     return "unsupported";
    case M64ERR_WRONG_TYPE:      return "wrong_type";
    default:                     return "unknown";
    }
}

enum log_level level_from_m64(m64p_msg_level level)
{
    switch (level) {
    case M64MSG_ERROR:   return L_ERR;
    case M64MSG_WARNING: return L_WARN;
    default:             return L_INFO;
    }
}

static const char *
error_level_to_name(enum log_level level, bool short_name = false)
{
    if (short_name) {
        switch (level) {
        case L_ERR:  return "ERROR";
        case L_WARN: return "Warn";
        case L_INFO: return "info";
        default:     return "";
        }
    } else {
        switch (level) {
        case L_ERR:  return "Error";
        case L_WARN: return "Warning";
        case L_INFO: return "Information";
        default:     return "";
        }
    }
}

static void log_to_console(enum log_level level, const char *from,
        const char *msg, const char *details)
{
    bool do_color = true;
    const char *color = "";
    switch (level) {
    case L_ERR:  color = "91"; break;
    case L_WARN: color = "93"; break;
    }
    if (do_color) {
        printf("\x1b[%sm", color);
    }
    const char *level_str = error_level_to_name(level, true);
    printf("[%s] %s: %s", from, level_str, msg);
    if (details) {
        printf(" (%s)", details);
    }
    if (do_color) {
        printf("\x1b[m");
    }
    printf("\n");
}

void log_error(enum log_level level, const char *from,
        const char *msg, const char *details)
{
    log_to_console(level, from, msg, details);
}

static QMessageBox::Icon error_level_to_qt_icon(enum log_level level)
{
    switch (level) {
    case L_ERR: return QMessageBox::Critical;
    case L_WARN: return QMessageBox::Warning;
    case L_INFO: return QMessageBox::Information;
    default: return QMessageBox::NoIcon;
    }
}

void show_error(enum log_level level, const char *from,
        const char *msg, const char *details)
{
    QMessageBox msgbox;
    msgbox.setIcon(error_level_to_qt_icon(level));
    msgbox.setWindowTitle(error_level_to_name(level));
    QString qmsg(msg);
    if (details) {
        qmsg = qmsg + "\n\nDetails:\n" + details;
    }
    msgbox.setText(qmsg);
    msgbox.exec();
}

void log_and_show_error(enum log_level level, const char *from,
        const char *msg, const char *details)
{
    log_error(level, from, msg, details);
    show_error(level, from, msg, details);
}
