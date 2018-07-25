#include "emucontroller.h"
#include "emuthread.h"
#include "emu.h"
#include "../core.h"

EmuThread *emuthread = NULL;


EmuController::EmuController()
{
}


void EmuController::startGame(const QString &romFileName, const QString &zipFileName)
{
    emuthread = new EmuThread(romFileName, zipFileName);
    emuthread->start();
}


bool EmuController::isExecuting()
{
    m64p_error rval;
    int state;
    rval = CoreDoCommand(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &state);
    return state != M64EMU_STOPPED;
}


void EmuController::emitResumed()
{
    emit resumed();
}


void EmuController::emitPaused()
{
    emit paused();
}


void EmuController::stopGame()
{
    Emu::stopGame();
}


void EmuController::play()
{
    Emu::play();
}


void EmuController::pause()
{
    Emu::pause();
}
