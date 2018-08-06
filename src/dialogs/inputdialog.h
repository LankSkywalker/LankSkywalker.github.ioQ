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

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "keyspec.h"

#include <m64p_types.h>

#include <vector>
#include <QDialog>
#include <QLabel>
#include <SDL.h>
class QAbstractButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLineEdit;

namespace Ui {
    class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

    struct Button {
        const char *configName;
        QPushButton *button;
    };

    struct Value {
        const char *configName;
        std::vector<KeySpec> keys;
    };

    struct InputReadingState {
        // If this is false, the rest of the fields must not be used.
        bool reading;
        Button *button;
        Value *value;
        SDL_Joystick *joy;
    };

    struct Controller {
        QString sectionName;
        m64p_handle configHandle;
        std::vector<Value> values;
    };

public:
    explicit InputDialog(const QString &name, QWidget *parent = NULL);
    ~InputDialog();

private slots:
    void accept();
    void controllerSelected(int index);

private:
    Controller &currentController();
    void saveController(int controllerIndex);
    void getButtons();
    void setValues();
    void connectButtons();
    void startReadInput(Button &b, Value &v);
    void stopReadInput();
    void timerEvent(QTimerEvent *timerEvent) override;
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void loadUnloadPlugin(const char *name);

    bool sdlWasInited;
    int inputTimer;
    InputReadingState inputReadingState;
    Ui::InputDialog *ui;
    const QString pluginName;
    int currentControllerIndex = 0;
    std::vector<Controller> controllers;
    std::vector<Button> buttons;
};


#endif // INPUTDIALOG_H
