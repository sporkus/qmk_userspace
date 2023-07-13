#pragma once

#include QMK_KEYBOARD_H
#include "quantum/keycodes.h"
#include "layout.h"

#define LAYOUT_w(...)        LAYOUT_2x2u(__VA_ARGS__)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_w(
                          LEFT_ROW1(_BASE),        KC_TAB,    KC_BSPC,    RIGHT_ROW1(_BASE),        \
                          HRML(LEFT_ROW2(_BASE)),  NAV_ESC,   KC_COLN,    HRMR(RIGHT_ROW2(_BASE)),  \
                          BRML(LEFT_ROW3(_BASE)),  KC_LSFT,   SFT_ENT,    BRMR(RIGHT_ROW3(_BASE)),  \
     KC_LCTL,  KC_LALT,   LEFT_THUMB(_BASE),       _______,   _______,    RIGHT_THUMB(_BASE),  KC_RALT, KC_RCTL

    ),

    [1] = LAYOUT_w(
                          LEFT_ROW1(_NUM),        _______,   _______,    RIGHT_ROW1(_NUM),        \
                          LEFT_ROW2(_NUM),        _______,   _______,    RIGHT_ROW2(_NUM),        \
                          LEFT_ROW3(_NUM),        _______,   _______,    RIGHT_ROW3(_NUM),        \
     _______,  _______,   LEFT_THUMB(_NUM),       _______,   _______,    RIGHT_THUMB(_NUM),  _______, _______
    ),

    [2] = LAYOUT_w(
                          LEFT_ROW1(_NAV),        _______,   _______,    RIGHT_ROW1(_NAV),        \
                          LEFT_ROW2(_NAV),        _______,   _______,    RIGHT_ROW2(_NAV),        \
                          LEFT_ROW3(_NAV),        _______,   _______,    RIGHT_ROW3(_NAV),        \
     _______,  _______,   LEFT_THUMB(_NAV),       _______,   _______,    RIGHT_THUMB(_NAV),  _______, _______
    ),

    [3] = LAYOUT_w(
                          LEFT_ROW1(_NAV),        _______,   _______,    RIGHT_ROW1(_NAV),        \
                          LEFT_ROW2(_NAV),        _______,   _______,    RIGHT_ROW2(_NAV),        \
                          LEFT_ROW3(_NAV),        _______,   _______,    RIGHT_ROW3(_NAV),        \
     _______,  _______,   LEFT_THUMB(_NAV),       _______,   _______,    RIGHT_THUMB(_NAV),  _______, _______

    )
};

