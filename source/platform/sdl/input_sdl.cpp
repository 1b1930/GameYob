#ifdef BACKEND_SDL

#include <string.h>

#include "platform/common/config.h"
#include "platform/gfx.h"
#include "platform/input.h"

#include <SDL2/SDL_events.h>

static KeyConfig defaultKeyConfig = {
        "Main",
        {FUNC_KEY_NONE}
};

static KeyConfig* currKeyConfig = &defaultKeyConfig;

static bool pressed[512] = {false};
static bool held[512] = {false};
static bool forceReleased[NUM_FUNC_KEYS] = {false};

static long nextRepeat = 0;

static const Uint8* keyState = NULL;
static int keyCount = 0;

void inputInit() {
    defaultKeyConfig.funcKeys[SDL_SCANCODE_Z] = FUNC_KEY_A;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_X] = FUNC_KEY_B;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_LEFT] = FUNC_KEY_LEFT;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_RIGHT] = FUNC_KEY_RIGHT;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_UP] = FUNC_KEY_UP;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_DOWN] = FUNC_KEY_DOWN;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_RETURN] = FUNC_KEY_START;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_RSHIFT] = FUNC_KEY_SELECT;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_M] = FUNC_KEY_MENU;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_P] = FUNC_KEY_MENU_PAUSE;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_S] = FUNC_KEY_SAVE;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_Q] = FUNC_KEY_AUTO_A;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_W] = FUNC_KEY_AUTO_B;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_LCTRL] = FUNC_KEY_FAST_FORWARD;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_LALT] = FUNC_KEY_FAST_FORWARD_TOGGLE;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_RCTRL] = FUNC_KEY_SCALE;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_RALT] = FUNC_KEY_RESET;
    defaultKeyConfig.funcKeys[SDL_SCANCODE_BACKSPACE] = FUNC_KEY_SCREENSHOT;

    keyState = SDL_GetKeyboardState(&keyCount);
}

void inputCleanup() {
}

void inputUpdate() {
    for(int key = 0; key < keyCount; key++) {
        if(keyState[key] == 1) {
            pressed[key] = !held[key];
            held[key] = true;
        } else {
            pressed[key] = false;
            held[key] = false;
        }
    }

    for(int key = 0; key < NUM_FUNC_KEYS; key++) {
        bool currPressed = false;
        for(int i = 0; i < keyCount; i++) {
            if(keyState[i] == 1 && key == currKeyConfig->funcKeys[i]) {
                currPressed = true;
                break;
            }
        }

        if(forceReleased[key] && !currPressed) {
            forceReleased[key] = false;
        }
    }
}

int inputGetKeyCount() {
    return keyCount;
}

const char* inputGetKeyName(int keyIndex) {
    return SDL_GetScancodeName((SDL_Scancode) keyIndex);
}

bool inputIsValidKey(int keyIndex) {
    return keyIndex >= 0 && keyIndex < keyCount && keyIndex != SDL_SCANCODE_RETURN2 && strcmp(SDL_GetScancodeName((SDL_Scancode) keyIndex), "") != 0;
}

bool inputKeyHeld(int key) {
    if(key < 0 || key >= NUM_FUNC_KEYS) {
        return false;
    }

    if(forceReleased[key]) {
        return false;
    }

    for(int i = 0; i < keyCount; i++) {
        if(held[i] && key == currKeyConfig->funcKeys[i]) {
            return true;
        }
    }

    return false;
}

bool inputKeyPressed(int key) {
    if(key < 0 || key >= NUM_FUNC_KEYS) {
        return false;
    }

    if(forceReleased[key]) {
        return false;
    }

    for(int i = 0; i < keyCount; i++) {
        if(pressed[i] && key == currKeyConfig->funcKeys[i]) {
            return true;
        }
    }

    return false;
}

bool inputKeyRepeat(int key) {
    if(key < 0 || key >= NUM_FUNC_KEYS) {
        return false;
    }

    if(inputKeyPressed(key)) {
        nextRepeat = time(NULL) + 250;
        return true;
    }

    if(inputKeyHeld(key) && time(NULL) >= nextRepeat) {
        nextRepeat = time(NULL) + 50;
        return true;
    }

    return false;
}

void inputKeyRelease(int key) {
    if(key < 0 || key >= NUM_FUNC_KEYS) {
        return;
    }

    forceReleased[key] = true;
}

void inputReleaseAll() {
    for(int i = 0; i < NUM_FUNC_KEYS; i++) {
        inputKeyRelease(i);
    }
}

u16 inputGetMotionSensorX() {
    return 2047;
}

u16 inputGetMotionSensorY() {
    return 2047;
}

KeyConfig inputGetDefaultKeyConfig() {
    return defaultKeyConfig;
}

void inputLoadKeyConfig(KeyConfig* keyConfig) {
    currKeyConfig = keyConfig;
}

#endif
