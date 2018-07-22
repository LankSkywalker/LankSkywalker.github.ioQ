#include "emucontroller.h"
#include "emuthread.h"
#include "emu.h"

EmuThread *emuthread = NULL;


EmuController::EmuController()
{
}


void EmuController::startGame(const QString &romFileName, const QString &zipFileName)
{
    emuthread = new EmuThread(romFileName, zipFileName);
    emuthread->start();
}


void EmuController::stopGame()
{
    Emu::stopGame();
}
