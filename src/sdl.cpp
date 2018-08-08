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

#include "sdl.h"

#include <QKeyEvent>


struct SdlKey {
    SDL_Scancode scancode;
    SDL_Keycode keycode;
};

static SdlKey qtToSdlHigh[] = {
    {SDL_SCANCODE_ESCAPE,       SDLK_ESCAPE},       // Qt::Key_Escape
    {SDL_SCANCODE_TAB,          SDLK_TAB},          // Qt::Key_Tab
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Backtab
    {SDL_SCANCODE_BACKSPACE,    SDLK_BACKSPACE},    // Qt::Key_Backspace
    {SDL_SCANCODE_RETURN,       SDLK_RETURN},       // Qt::Key_Return
    {SDL_SCANCODE_RETURN2,      SDLK_RETURN2},      // Qt::Key_Enter
    {SDL_SCANCODE_INSERT,       SDLK_INSERT},       // Qt::Key_Insert
    {SDL_SCANCODE_DELETE,       SDLK_DELETE},       // Qt::Key_Delete
    {SDL_SCANCODE_PAUSE,        SDLK_PAUSE},        // Qt::Key_Pause
    {SDL_SCANCODE_PRINTSCREEN,  SDLK_PRINTSCREEN},  // Qt::Key_Print
    {SDL_SCANCODE_SYSREQ,       SDLK_SYSREQ},       // Qt::Key_SysReq
    {SDL_SCANCODE_CLEAR,        SDLK_CLEAR},        // Qt::Key_Clear
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_HOME,         SDLK_HOME},         // Qt::Key_Home
    {SDL_SCANCODE_END,          SDLK_END},          // Qt::Key_End
    {SDL_SCANCODE_LEFT,         SDLK_LEFT},         // Qt::Key_Left
    {SDL_SCANCODE_UP,           SDLK_UP},           // Qt::Key_Up
    {SDL_SCANCODE_RIGHT,        SDLK_RIGHT},        // Qt::Key_Right
    {SDL_SCANCODE_DOWN,         SDLK_DOWN},         // Qt::Key_Down
    {SDL_SCANCODE_PAGEUP,       SDLK_PAGEUP},       // Qt::Key_PageUp
    {SDL_SCANCODE_PAGEDOWN,     SDLK_PAGEDOWN},     // Qt::Key_PageDown
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_LSHIFT,       SDLK_LSHIFT},       // Qt::Key_Shift
    {SDL_SCANCODE_LCTRL,        SDLK_LCTRL},        // Qt::Key_Control
    {SDL_SCANCODE_LGUI,         SDLK_LGUI},         // Qt::Key_Meta
    {SDL_SCANCODE_LALT,         SDLK_LALT},         // Qt::Key_Alt
    {SDL_SCANCODE_CAPSLOCK,     SDLK_CAPSLOCK},     // Qt::Key_CapsLock
    {SDL_SCANCODE_NUMLOCKCLEAR, SDLK_NUMLOCKCLEAR}, // Qt::Key_NumLock
    {SDL_SCANCODE_SCROLLLOCK,   SDLK_SCROLLLOCK},   // Qt::Key_ScrollLock
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_F1,           SDLK_F1},           // Qt::Key_F1
    {SDL_SCANCODE_F2,           SDLK_F2},           // Qt::Key_F2
    {SDL_SCANCODE_F3,           SDLK_F3},           // Qt::Key_F3
    {SDL_SCANCODE_F4,           SDLK_F4},           // Qt::Key_F4
    {SDL_SCANCODE_F5,           SDLK_F5},           // Qt::Key_F5
    {SDL_SCANCODE_F6,           SDLK_F6},           // Qt::Key_F6
    {SDL_SCANCODE_F7,           SDLK_F7},           // Qt::Key_F7
    {SDL_SCANCODE_F8,           SDLK_F8},           // Qt::Key_F8
    {SDL_SCANCODE_F9,           SDLK_F9},           // Qt::Key_F9
    {SDL_SCANCODE_F10,          SDLK_F10},          // Qt::Key_F10
    {SDL_SCANCODE_F11,          SDLK_F11},          // Qt::Key_F11
    {SDL_SCANCODE_F12,          SDLK_F12},          // Qt::Key_F12
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F13
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F14
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F15
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F16
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F17
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F18
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F19
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F20
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F21
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F22
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F23
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F24
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F25
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F26
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F27
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F28
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F29
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F30
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F31
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F32
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F33
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F34
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_F35
    {SDL_SCANCODE_LGUI,         SDLK_LGUI},         // Qt::Key_Super_L
    {SDL_SCANCODE_RGUI,         SDLK_RGUI},         // Qt::Key_Super_R
    {SDL_SCANCODE_MENU,         SDLK_MENU},         // Qt::Key_Menu
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Hyper_L
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Hyper_R
    {SDL_SCANCODE_HELP,         SDLK_HELP},         // Qt::Key_Help
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Direction_L
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Direction_R
};

static SdlKey qtToSdl[] = {
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // (none)
    {SDL_SCANCODE_SPACE,        SDLK_SPACE},        // Qt::Key_Space
    {SDL_SCANCODE_KP_EXCLAM,    SDLK_KP_EXCLAM},    // Qt::Key_Exclam
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_QuoteDbl
    {SDL_SCANCODE_KP_HASH,      SDLK_KP_HASH},      // Qt::Key_NumberSign
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Dollar
    {SDL_SCANCODE_KP_PERCENT,   SDLK_KP_PERCENT},   // Qt::Key_Percent
    {SDL_SCANCODE_KP_AMPERSAND, SDLK_KP_AMPERSAND}, // Qt::Key_Ampersand
    {SDL_SCANCODE_APOSTROPHE,   SDLK_QUOTE},        // Qt::Key_Apostrophe
    {SDL_SCANCODE_KP_LEFTPAREN, SDLK_KP_LEFTPAREN}, // Qt::Key_ParenLeft
    {SDL_SCANCODE_KP_RIGHTPAREN,SDLK_KP_RIGHTPAREN},// Qt::Key_ParenRight
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Asterisk
    {SDL_SCANCODE_KP_PLUS,      SDLK_KP_PLUS},      // Qt::Key_Plus
    {SDL_SCANCODE_COMMA,        SDLK_COMMA},        // Qt::Key_Comma
    {SDL_SCANCODE_MINUS,        SDLK_MINUS},        // Qt::Key_Minus
    {SDL_SCANCODE_PERIOD,       SDLK_PERIOD},       // Qt::Key_Period
    {SDL_SCANCODE_SLASH,        SDLK_SLASH},        // Qt::Key_Slash
    {SDL_SCANCODE_0,            SDLK_0},            // Qt::Key_0
    {SDL_SCANCODE_1,            SDLK_1},            // Qt::Key_1
    {SDL_SCANCODE_2,            SDLK_2},            // Qt::Key_2
    {SDL_SCANCODE_3,            SDLK_3},            // Qt::Key_3
    {SDL_SCANCODE_4,            SDLK_4},            // Qt::Key_4
    {SDL_SCANCODE_5,            SDLK_5},            // Qt::Key_5
    {SDL_SCANCODE_6,            SDLK_6},            // Qt::Key_6
    {SDL_SCANCODE_7,            SDLK_7},            // Qt::Key_7
    {SDL_SCANCODE_8,            SDLK_8},            // Qt::Key_8
    {SDL_SCANCODE_9,            SDLK_9},            // Qt::Key_9
    {SDL_SCANCODE_KP_COLON,     SDLK_KP_COLON},     // Qt::Key_Colon
    {SDL_SCANCODE_SEMICOLON,    SDLK_SEMICOLON},    // Qt::Key_Semicolon
    {SDL_SCANCODE_KP_LESS,      SDLK_KP_LESS},      // Qt::Key_Less
    {SDL_SCANCODE_EQUALS,       SDLK_EQUALS},       // Qt::Key_Equal
    {SDL_SCANCODE_KP_GREATER,   SDLK_KP_GREATER},   // Qt::Key_Greater
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Question
    {SDL_SCANCODE_KP_AT,        SDLK_KP_AT},        // Qt::Key_At
    {SDL_SCANCODE_A,            SDLK_a},            // Qt::Key_A
    {SDL_SCANCODE_B,            SDLK_b},            // Qt::Key_B
    {SDL_SCANCODE_C,            SDLK_c},            // Qt::Key_C
    {SDL_SCANCODE_D,            SDLK_d},            // Qt::Key_D
    {SDL_SCANCODE_E,            SDLK_e},            // Qt::Key_E
    {SDL_SCANCODE_F,            SDLK_f},            // Qt::Key_F
    {SDL_SCANCODE_G,            SDLK_g},            // Qt::Key_G
    {SDL_SCANCODE_H,            SDLK_h},            // Qt::Key_H
    {SDL_SCANCODE_I,            SDLK_i},            // Qt::Key_I
    {SDL_SCANCODE_J,            SDLK_j},            // Qt::Key_J
    {SDL_SCANCODE_K,            SDLK_k},            // Qt::Key_K
    {SDL_SCANCODE_L,            SDLK_l},            // Qt::Key_L
    {SDL_SCANCODE_M,            SDLK_m},            // Qt::Key_M
    {SDL_SCANCODE_N,            SDLK_n},            // Qt::Key_N
    {SDL_SCANCODE_O,            SDLK_o},            // Qt::Key_O
    {SDL_SCANCODE_P,            SDLK_p},            // Qt::Key_P
    {SDL_SCANCODE_Q,            SDLK_q},            // Qt::Key_Q
    {SDL_SCANCODE_R,            SDLK_r},            // Qt::Key_R
    {SDL_SCANCODE_S,            SDLK_s},            // Qt::Key_S
    {SDL_SCANCODE_T,            SDLK_t},            // Qt::Key_T
    {SDL_SCANCODE_U,            SDLK_u},            // Qt::Key_U
    {SDL_SCANCODE_V,            SDLK_v},            // Qt::Key_V
    {SDL_SCANCODE_W,            SDLK_w},            // Qt::Key_W
    {SDL_SCANCODE_X,            SDLK_x},            // Qt::Key_X
    {SDL_SCANCODE_Y,            SDLK_y},            // Qt::Key_Y
    {SDL_SCANCODE_Z,            SDLK_z},            // Qt::Key_Z
    {SDL_SCANCODE_LEFTBRACKET,  SDLK_LEFTBRACKET},  // Qt::Key_BracketLeft
    {SDL_SCANCODE_BACKSLASH,    SDLK_BACKSLASH},    // Qt::Key_Backslash
    {SDL_SCANCODE_RIGHTBRACKET, SDLK_RIGHTBRACKET}, // Qt::Key_BracketRight
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_AsciiCircum
    {SDL_SCANCODE_UNKNOWN,      SDLK_UNKNOWN},      // Qt::Key_Underscore
    {SDL_SCANCODE_GRAVE,        SDLK_BACKQUOTE},    // Qt::Key_QuoteLeft
};


static SdlKey getSdlKey(unsigned key)
{
    SdlKey sdlKey = {SDL_SCANCODE_UNKNOWN, SDLK_UNKNOWN};
    if (key >= 0x1000000) {
        key = key & 0xffffff;
        if (key < sizeof qtToSdlHigh / sizeof *qtToSdlHigh) {
            sdlKey = qtToSdlHigh[key];
        }
    } else {
        if (key < sizeof qtToSdl / sizeof *qtToSdl) {
            sdlKey = qtToSdl[key];
        }
    }
    return sdlKey;
}


SDL_Scancode qtToSdlScancode(const QKeyEvent *keyEvent)
{
    return getSdlKey(keyEvent->key()).scancode;
}


int qtToSdlKey(const QKeyEvent *keyEvent)
{
    return getSdlKey(keyEvent->key()).keycode;
}


const char *sdlKeyName(SDL_Keycode keycode)
{
    return SDL_GetKeyName(keycode);
}
