#ifndef CORE_H
#define CORE_H

#define M64P_CORE_PROTOTYPES
#include <m64p_common.h>
#include <m64p_frontend.h>
#include <m64p_types.h>

class Core
{
public:
    Core();
    ~Core();
    bool init();
    static Core &get();
    m64p_dynlib_handle get_libhandle() const;

private:
    Core(const Core &other);
    Core &operator=(const Core &other);
    static Core *instance;
    m64p_dynlib_handle libhandle;
};

#endif // CORE_H
