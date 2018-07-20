#ifdef Q_OS_WIN
#include "osal_dynamiclib_win32.c"
#else
#include "osal_dynamiclib_unix.c"
#endif
