#pragma once

#include "quantum/keycodes.h"
#include "layout.h"

// This file should not be called [keyboard].h as it will cause conflict with qmk
// layout to be used in [keyboard].json | exact layout name defined in keyboards/[keyboard]/info.json
#define LAYOUT_2x2u_w(...)        LAYOUT_2x2u(__VA_ARGS__)

// layers
#define BASE_LAYER \
                          LEFT_ROW1(_BASE),        KC_TAB,    KC_BSPC,    RIGHT_ROW1(_BASE),        \
                          HRML(LEFT_ROW2(_BASE)),  NAV_ESC,   KC_COLN,    HRMR(RIGHT_ROW2(_BASE)),  \
                          BRML(LEFT_ROW3(_BASE)),  KC_LSFT,   SFT_ENT,    BRMR(RIGHT_ROW3(_BASE)),  \
     KC_LCTL,  KC_LALT,   LEFT_THUMB(_BASE),       _______,   _______,    RIGHT_THUMB(_BASE),  KC_RALT, KC_RCTL

#define NUM_LAYER \
                          LEFT_ROW1(_NUM),        _______,   _______,    RIGHT_ROW1(_NUM),        \
                          LEFT_ROW2(_NUM),        _______,   _______,    RIGHT_ROW2(_NUM),        \
                          LEFT_ROW3(_NUM),        _______,   _______,    RIGHT_ROW3(_NUM),        \
     _______,  _______,   LEFT_THUMB(_NUM),       _______,   _______,    RIGHT_THUMB(_NUM),  _______, _______

#define NAV_LAYER \
                          LEFT_ROW1(_NAV),        _______,   _______,    RIGHT_ROW1(_NAV),        \
                          LEFT_ROW2(_NAV),        _______,   _______,    RIGHT_ROW2(_NAV),        \
                          LEFT_ROW3(_NAV),        _______,   _______,    RIGHT_ROW3(_NAV),        \
     _______,  _______,   LEFT_THUMB(_NAV),       _______,   _______,    RIGHT_THUMB(_NAV),  _______, _______

#define FN_LAYER \
                          LEFT_ROW1(_FN),        _______,   _______,    RIGHT_ROW1(_FN),        \
                          LEFT_ROW2(_FN),        _______,   _______,    RIGHT_ROW2(_FN),        \
                          LEFT_ROW3(_FN),        _______,   _______,    RIGHT_ROW3(_FN),        \
     _______,  _______,   LEFT_THUMB(_FN),       _______,   _______,    RIGHT_THUMB(_FN),  _______, _______
