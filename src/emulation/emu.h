#ifndef EMU_H
#define EMU_H

class QString;

namespace Emu
{
    void runGame(const QString &romFileName, const QString &zipFileName);
    void stopGame();
}

#endif // EMU_H
