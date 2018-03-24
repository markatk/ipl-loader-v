/*
* File: keyboard.cpp
* Date: 23.03.2018
* Author: MarkAtk
*
* MIT License
*
* Copyright (c) 2018 IPLLoaderV
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "keyboard.h"

const int KEYS_SIZE = 255;

struct {
    DWORD time;
    BOOL isWithAlt;
    BOOL wasDownBefore;
    BOOL isUpNow;
} keyStates[KEYS_SIZE];

void onKeyboardMessage(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow) {
    if (key < KEYS_SIZE) {
        keyStates[key].time = GetTickCount();
        keyStates[key].isWithAlt = isWithAlt;
        keyStates[key].wasDownBefore = wasDownBefore;
        keyStates[key].isUpNow = isUpNow;
    }
}

const int NOW_PERIOD = 100, MAX_DOWN = 5000; // ms

bool isKeyPressed(DWORD key) {
    if (key >= KEYS_SIZE) {
        return false;
    }

    return (GetTickCount() < keyStates[key].time + MAX_DOWN) && !keyStates[key].isUpNow;
}

bool isKeyReleased(DWORD key, bool exclusive) {
    if (key >= KEYS_SIZE) {
        return false;
    }

    bool b = GetTickCount() < keyStates[key].time + NOW_PERIOD && keyStates[key].isUpNow;
    if (b && exclusive) {
        resetKeyState(key);
    }
        
    return b;
}

void resetKeyState(DWORD key) {
    if (key < KEYS_SIZE) {
        memset(&keyStates[key], 0, sizeof(keyStates[0]));
    }
}
