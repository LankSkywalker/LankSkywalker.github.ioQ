#include "glwindow.h"

void GlWindow::initializeGL()
{
    extern EmuThread *emuthread;
    doneCurrent();
    context()->moveToThread(emuthread);
}
