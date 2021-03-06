/*
 * File: IPLLoaderV.cpp
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

#include "script.h"

#include "keyboard.h"

#include "main.h"
#include "natives.h"
#include "types.h"
#include "enums.h"

#include <stdlib.h>
#include <string>

bool menuVisible = false;
int selectedMenuItem = 0;
bool mapsToggled;

const int MENU_ITEM_COUNT = 6;
const float MENU_WIDTH = 250;

char *itemTexts[MENU_ITEM_COUNT] = { "Load IPL", "Unload IPL", "Enable Interior Prop", "Disable Interior Prop", "Teleport", "Toggle MP DLC Maps"  };

void menuBeep() {
    AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
}

void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7) {
    GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

void drawMenuItem(const char *caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool title, bool rescaleText = true) {
    // default values
    int text_col[4] = { 255, 255, 255, 255 },
        rect_col[4] = { 70, 95, 95, 255 };
    float text_scale = 0.35f;
    int font = 0;

    // correcting values for active line
    if (active) {
        text_col[0] = 0;
        text_col[1] = 0;
        text_col[2] = 0;

        rect_col[0] = 218;
        rect_col[1] = 242;
        rect_col[2] = 216;

        if (rescaleText) text_scale = 0.40f;
    }

    if (title) {
        rect_col[0] = 0;
        rect_col[1] = 0;
        rect_col[2] = 0;

        if (rescaleText) {
            text_scale = 0.50;
        }

        font = 1;
    }

    int screen_w, screen_h;
    GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

    textLeft += lineLeft;

    float lineWidthScaled = lineWidth / (float)screen_w;
    float lineTopScaled = lineTop / (float)screen_h;
    float textLeftScaled = textLeft / (float)screen_w;
    float lineHeightScaled = lineHeight / (float)screen_h;

    float lineLeftScaled = lineLeft / (float)screen_w;

    // text upper part
    UI::SET_TEXT_FONT(font);
    UI::SET_TEXT_SCALE(0.0, text_scale);
    UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
    UI::SET_TEXT_CENTRE(0);
    UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
    UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
    UI::_SET_TEXT_ENTRY("STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption);
    UI::_DRAW_TEXT(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

    // text lower part
    UI::SET_TEXT_FONT(font);
    UI::SET_TEXT_SCALE(0.0, text_scale);
    UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
    UI::SET_TEXT_CENTRE(0);
    UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
    UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
    UI::_SET_TEXT_GXT_ENTRY("STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption);
    int num25 = UI::_0x9040DFB09BE75706(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

    // rect
    drawRect(lineLeftScaled, lineTopScaled + (0.00278f), lineWidthScaled, ((((float)(num25)* UI::_0xDB88A37483346780(text_scale, 0)) + (lineHeightScaled * 2.0f)) + 0.005f), rect_col[0], rect_col[1], rect_col[2], rect_col[3]);
}

void drawMenu() {
    drawMenuItem("IPLLoaderV", MENU_WIDTH, 15, 18, 0, 5, false, true);

    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        if (selectedMenuItem == i) {
            drawMenuItem(itemTexts[i], MENU_WIDTH + 1, 11, 56 + i * 36, 0, 7, true, false);
        } else {
            drawMenuItem(itemTexts[i], MENU_WIDTH, 9, 60 + i * 36, 0, 9, false, false);
        }
    }
}

char *getText(char *title) {
    GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, title, "", "", "", "", "", 64);

    while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
        scriptWait(0);
    }

    return GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
}

void showNotification(char *text) {
    UI::_SET_NOTIFICATION_TEXT_ENTRY("CELL_EMAIL_BCON");
    UI::_ADD_TEXT_COMPONENT_STRING(text);
    UI::_DRAW_NOTIFICATION(false, true);
}

void handleMenu() {
    char *result;
    auto playerPed = PLAYER::PLAYER_PED_ID();
    auto position = ENTITY::GET_ENTITY_COORDS(playerPed, true);
    int interior;
    int x, y, z;

    switch (selectedMenuItem) {
        case 0:
            result = getText(itemTexts[selectedMenuItem]);
            if (result == NULL) {
                return;
            }

            if (STREAMING::IS_IPL_ACTIVE(result) == false) {
                STREAMING::REQUEST_IPL(result);
            }
            break;

        case 1:
            result = getText(itemTexts[selectedMenuItem]);
            if (result == NULL) {
                return;
            }

            if (STREAMING::IS_IPL_ACTIVE(result)) {
                STREAMING::REMOVE_IPL(result);
            }
            break;

        case 2:
            result = getText(itemTexts[selectedMenuItem]);
            if (result == NULL) {
                return;
            }

            interior = INTERIOR::GET_INTERIOR_AT_COORDS(position.x, position.y, position.z);
            if (INTERIOR::_IS_INTERIOR_PROP_ENABLED(interior, result) == false) {
                INTERIOR::_ENABLE_INTERIOR_PROP(interior, result);
                INTERIOR::REFRESH_INTERIOR(interior);
            }
            break;

        case 3:
            result = getText(itemTexts[selectedMenuItem]);
            if (result == NULL) {
                return;
            }

            interior = INTERIOR::GET_INTERIOR_AT_COORDS(position.x, position.y, position.z);

            char buffer[10];
            _itoa_s(interior, buffer, 10);
            showNotification(buffer);

            if (INTERIOR::_IS_INTERIOR_PROP_ENABLED(interior, result)) {
                INTERIOR::_DISABLE_INTERIOR_PROP(interior, result);
                INTERIOR::REFRESH_INTERIOR(interior);
            }
            break;

        case 4:
            result = getText(itemTexts[selectedMenuItem]);
            if (result == NULL) {
                return;
            }

            // get coords
            x = strtol(result, &result, 10);
            y = strtol(result, &result, 10);
            z = strtol(result, &result, 10);
            ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, x, y, z, false, false, true);
            break;

        case 5:
            mapsToggled = !mapsToggled;
            GAMEPLAY::_ENABLE_MP_DLC_MAPS(mapsToggled);
            break;

        default:
            break;
    }
}

void scriptMain() {
    mapsToggled = true;
    GAMEPLAY::_ENABLE_MP_DLC_MAPS(true);
    DLC2::_LOAD_MP_DLC_MAPS();

    while (true) {
        if (isKeyReleased(VK_F3)) {
            menuBeep();
            menuVisible = true;
        }

        if (menuVisible) {
            if (isKeyReleased(VK_UP) || isKeyReleased(VK_NUMPAD8)) {
                selectedMenuItem--;
                if (selectedMenuItem < 0) {
                    selectedMenuItem = MENU_ITEM_COUNT - 1;
                }

                menuBeep();
            } else if (isKeyReleased(VK_DOWN) || isKeyReleased(VK_NUMPAD2)) {
                selectedMenuItem++;
                if (selectedMenuItem >= MENU_ITEM_COUNT) {
                    selectedMenuItem = 0;
                }

                menuBeep();
            } else if (isKeyReleased(VK_BACK) || isKeyReleased(VK_NUMPAD0)) {
                menuVisible = false;
                menuBeep();
            } else if (isKeyReleased(VK_RETURN) || isKeyReleased(VK_NUMPAD5)) {
                menuVisible = false;
                menuBeep();

                handleMenu();
            }

            drawMenu();
        }

        scriptWait(0);
    }
}
