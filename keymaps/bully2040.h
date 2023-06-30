#pragma once

#include "quantum/keycodes.h"
#include "layout.h"

// This file should not be called [keyboard].h as it will cause conflict with qmk
// layout to be used in [keyboard].json | exact layout name defined in keyboards/[keyboard]/info.json
#define LAYOUT_w(...)        LAYOUT(__VA_ARGS__)

// layers
#define BASE_LAYER \
          KC_TAB,              LEFT_ROW1(_BASE),        RIGHT_ROW1(_BASE),        KC_DEL,  KC_BSPC, \
          NAV_ESC,             HRML(LEFT_ROW2(_BASE)),  HRMR(RIGHT_ROW2(_BASE)),  KC_COLN, \
          KC_LSFT,             LEFT_ROW3(_BASE),        RIGHT_ROW3(_BASE),        SFT_ENT, \
KC_LCTL,  KC_LALT,  KC_LGUI,   NUM_SPC,                 NAV_SPC,   KC_RGUI, KC_RALT, KC_RCTL

#define NUM_LAYER \
           _______,             LEFT_ROW1(_NUM),        RIGHT_ROW1(_NUM),   KC_LBRC, KC_RBRC, \
           _______,             LEFT_ROW2(_NUM),        RIGHT_ROW2(_NUM),   _______, \
           _______,             LEFT_ROW3(_NUM),        RIGHT_ROW3(_NUM),   _______,          \
 _______,  _______,  _______,   _______,                KC_0,     KC_DOT,  _______, _______

#define NAV_LAYER \
           GUI_TAB,             LEFT_ROW1(_NAV),        RIGHT_ROW1(_NAV),   _______, _______, \
           _______,             LEFT_ROW2(_NAV),        RIGHT_ROW2(_NAV),   _______,          \
           CW_TOGG,             LEFT_ROW3(_NAV),        RIGHT_ROW3(_NAV),   _______,          \
_______,  _______,  _______,    KC_ENT,                 KC_TAB,   _______, _______,  _______

#define FN_LAYER \
           _______,              LEFT_ROW1(_FN),         RIGHT_ROW1(_FN),   KC_F11,  KC_F12, \
           _______,              LEFT_ROW2(_FN),         RIGHT_ROW2(_FN),   _______,          \
           _______,              LEFT_ROW3(_FN),         RIGHT_ROW3(_FN),   _______,          \
 _______,  _______,  _______,   _______,                 _______,           _______, _______, _______
