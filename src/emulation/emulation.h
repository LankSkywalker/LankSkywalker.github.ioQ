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

#ifndef EMULATION_H
#define EMULATION_H

#include <m64p_types.h>
#include <cstdlib>
#include <QObject>
class QSurfaceFormat;
class QString;

class Emulation : public QObject
{
    Q_OBJECT

public:
    void startGame(const QString &romFileName, const QString &zipFileName = "");
    void runGame(const QString &romFileName, const QString &zipFileName);
    bool isExecuting();
    void stopGame();
    void setSaveSlot(int n);
    void reset(bool hard);
    bool getRomSettings(size_t size, m64p_rom_settings *romSettings);
    bool restartInputPlugin();
    QString currentGameFile() const;

signals:
    void createGlWindow(QSurfaceFormat *format);
    void destroyGlWindow();
    void resize(int width, int height);
    void started();
    void resumed();
    void paused();
    void finished();
    void toggleFullscreen();

public slots:
    void play();
    void pause();
    void advanceFrame();
    void saveState();
    void loadState();
    void resetSoft();
    void resetHard();
    void sendKeyDown(int sdlKey);
    void sendKeyUp(int sdlKey);
};

#endif // EMULATION_H
