#ifndef ERROR_H
#define ERROR_H

#include <m64p_types.h>
#include <stdlib.h>
#include <QString>
#include <string>

#define TO_STR(x)  #x
#define TO_STR_(x) TO_STR(x)
#define MY__LINE__ TO_STR_(__LINE__)

enum LogLevel {
    L_ERR, L_WARN, L_INFO, L_VERB,
};

const char *m64errstr(m64p_error errorValue);

LogLevel levelFromM64(m64p_msg_level level);

void logError(LogLevel level, const char *from,
        const char *msg, const char *details = NULL);
void showError(LogLevel level, const char *from,
        const char *msg, const char *details = NULL);
void logAndShowError(LogLevel level, const char *from,
        const char *msg, const char *details = NULL);

#define FROM_UI "ui"

static inline std::string toString(const char *s)
{
    return std::string(s);
}
static inline std::string toString(const QString &s)
{
    return std::string(s.toUtf8().data());
}

#define LOG(level, from, msg) \
    logError(level, from, toString(msg).data());

#define SHOW(level, from, msg) \
    logAndShowError(level, from, toString(msg).data());

#define LOG_DBG(level, from, msg) \
    logError(level, from, toString(msg).data(), __FILE__ ":" MY__LINE__);

#define SHOW_DBG(level, from, msg) \
    logAndShowError(level, from, toString(msg).data(), __FILE__ ":" MY__LINE__);


#define LOG_E(msg)     LOG(L_ERR, FROM_UI, msg)
#define LOG_E_DBG(msg) LOG_DBG(L_ERR, FROM_UI, msg)
#define LOG_W(msg)     LOG(L_WARN, FROM_UI, msg)
#define LOG_W_DBG(msg) LOG_DBG(L_WARN, FROM_UI, msg)
#define LOG_I(msg)     LOG(L_INFO, FROM_UI, msg)
#define LOG_I_DBG(msg) LOG_DBG(L_INFO, FROM_UI, msg)
#define LOG_V(msg)     LOG(L_VERB, FROM_UI, msg)
#define LOG_V_DBG(msg) LOG_DBG(L_VERB, FROM_UI, msg)

#define SHOW_E(msg)     SHOW(L_ERR, FROM_UI, msg)
#define SHOW_E_DBG(msg) SHOW_DBG(L_ERR, FROM_UI, msg)
#define SHOW_W(msg)     SHOW(L_WARN, FROM_UI, msg)
#define SHOW_W_DBG(msg) SHOW_DBG(L_WARN, FROM_UI, msg)
#define SHOW_I(msg)     SHOW(L_INFO, FROM_UI, msg)
#define SHOW_I_DBG(msg) SHOW_DBG(L_INFO, FROM_UI, msg)
#define SHOW_V(msg)     SHOW(L_VERB, FROM_UI, msg)
#define SHOW_V_DBG(msg) SHOW_DBG(L_VERB, FROM_UI, msg)

#endif // ERROR_H
