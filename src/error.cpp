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

#include "error.h"

#include <QMessageBox>

static std::vector<LogLine> logLines;


const std::vector<LogLine> &getLogLines()
{
    return logLines;
}

const char *m64errstr(m64p_error errorValue)
{
    switch (errorValue) {
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

LogLevel levelFromM64(m64p_msg_level level)
{
    switch (level) {
    case M64MSG_ERROR:   return L_ERR;
    case M64MSG_WARNING: return L_WARN;
    case M64MSG_INFO:    return L_INFO;
    case M64MSG_STATUS:  return L_INFO;
    case M64MSG_VERBOSE: return L_VERB;
    }
}

const char *errorLevelToName(LogLevel level, bool shortName)
{
    if (shortName) {
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

static void logToConsole(LogLevel level, const char *from,
        const char *msg, const char *details)
{
    if (level > L_INFO) {
        return;
    }
    bool doColor = true;
    const char *color = "";
    switch (level) {
    case L_ERR:  color = "91"; break;
    case L_WARN: color = "93"; break;
    }
    if (doColor) {
        printf("\x1b[%sm", color);
    }
    const char *levelStr = errorLevelToName(level, true);
    printf("[%s] %s: %s", from, levelStr, msg);
    if (details) {
        printf(" (%s)", details);
    }
    if (doColor) {
        printf("\x1b[m");
    }
    printf("\n");
}

static void logToMemory(LogLevel level, const char *from,
        const char *msg, const char *details)
{
    if (level > L_INFO) {
        return;
    }
    logLines.push_back(LogLine(level, from, msg, details));
}

void logError(LogLevel level, const char *from,
        const char *msg, const char *details)
{
    logToConsole(level, from, msg, details);
    logToMemory(level, from, msg, details);
}

static QMessageBox::Icon errorLevelToQtIcon(LogLevel level)
{
    switch (level) {
    case L_ERR: return QMessageBox::Critical;
    case L_WARN: return QMessageBox::Warning;
    case L_INFO: return QMessageBox::Information;
    default: return QMessageBox::NoIcon;
    }
}

void showError(LogLevel level, const char *from,
        const char *msg, const char *details)
{
    QMessageBox msgbox;
    msgbox.setIcon(errorLevelToQtIcon(level));
    msgbox.setWindowTitle(errorLevelToName(level));
    QString qmsg(msg);
    if (details) {
        qmsg = qmsg + "\n\nDetails:\n" + details;
    }
    msgbox.setText(qmsg);
    msgbox.exec();
}

void logAndShowError(LogLevel level, const char *from,
        const char *msg, const char *details)
{
    logError(level, from, msg, details);
    showError(level, from, msg, details);
}
