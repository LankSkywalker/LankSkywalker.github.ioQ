#include "emuthread.h"
#include "emucontroller.h"
#include "emu.h"


EmuThread::EmuThread(QString romFileName, QString zipFileName)
    : romFileName(romFileName)
    , zipFileName(zipFileName)
{
}


void EmuThread::run()
{
    Emu::runGame(romFileName, zipFileName);
}
