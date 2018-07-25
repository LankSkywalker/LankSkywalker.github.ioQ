#ifndef EMU_H
#define EMU_H

#include <m64p_types.h>
#include <cstdlib>
class QString;

namespace Emu
{
    void runGame(const QString &romFileName, const QString &zipFileName);
    void stopGame();
    void play();
    void pause();
    void advanceFrame();
    void saveState();
    void loadState();
    void setSaveSlot(int n);
    void reset(bool hard);
    bool getRomSettings(size_t size, m64p_rom_settings *romSettings);
}

#endif // EMU_H
