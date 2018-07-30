/***
 * Copyright (c) 2018, Robert Alm Nilsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the organization nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***/

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


void EmuController::advanceFrame()
{
    Emu::advanceFrame();
}


void EmuController::saveState()
{
    Emu::saveState();
}


void EmuController::loadState()
{
    Emu::loadState();
}


void EmuController::setSaveSlot(int n)
{
    Emu::setSaveSlot(n);
}


void EmuController::resetSoft()
{
    Emu::reset(false);
}


void EmuController::resetHard()
{
    Emu::reset(true);
}
