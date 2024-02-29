#pragma once

#include QMK_KEYBOARD_H
#include "quantum/keycodes.h"
#include "layout.h"

// This file should not be called [keyboard].h as it will cause conflict with qmk
// layout to be used in [keyboard].json | exact layout name defined in keyboards/[keyboard]/info.json
#define LAYOUT_w(...)        LAYOUT(__VA_ARGS__)

// Last row needs 7 params
// wkl dual thumb uses 1, 3, 5, 7
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_w(
        LEFT_ROW1(_BASE),            RIGHT_ROW1(_BASE), KC_BSPC,\
        HRML(LEFT_ROW2(_BASE)),      HRMR(RIGHT_ROW2(_BASE)),   \
        BRML(LEFT_ROW3(_BASE)),      BRMR(RIGHT_ROW3(_BASE)),   \
        KC_LCTL, LEFT_THUMB(_BASE),  XXX,  RIGHT_THUMB(_BASE), KC_LGUI
    ),
    [1] = LAYOUT_w(
        LEFT_ROW1(_NUM),             RIGHT_ROW1(_NUM), DELWORD,\
        LEFT_ROW2(_NUM),             RIGHT_ROW2(_NUM),         \
        LEFT_ROW3(_NUM),             RIGHT_ROW3(_NUM),         \
        _______, LEFT_THUMB(_NUM), XXX, RIGHT_THUMB(_NUM), _______
    ),
    [2] = LAYOUT_w(
        LEFT_ROW1(_NAV),             RIGHT_ROW1(_NAV), _______,\
        LEFT_ROW2(_NAV),             RIGHT_ROW2(_NAV),         \
        LEFT_ROW3(_NAV),             RIGHT_ROW3(_NAV),         \
        _______, LEFT_THUMB(_NAV), XXX, RIGHT_THUMB(_NAV), _______
    ),
    [3] = LAYOUT_w(
        LEFT_ROW1(_FN),              RIGHT_ROW1(_FN), _______,\
        LEFT_ROW2(_FN),              RIGHT_ROW2(_FN),         \
        LEFT_ROW3(_FN),              RIGHT_ROW3(_FN),         \
        _______, LEFT_THUMB(_FN), XXX, RIGHT_THUMB(_FN), _______
    )
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] =   { ENCODER_CCW_CW(RGB_HUD, RGB_HUI) },
    [2] =   { ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
    [3] =   { ENCODER_CCW_CW(RGB_VAD, RGB_VAI) }
};
#endif

void keyboard_post_init_kb(void) {
    keyboard_post_init_user();
}

