#ifndef EMU_H
#define EMU_H

class QString;

namespace Emu
{
    void runGame(const QString &romFileName, const QString &zipFileName);
    void stopGame();
    void play();
    void pause();
    void saveState();
    void loadState();
    void setSaveSlot(int n);
    void reset(bool hard);
}

#endif // EMU_H
