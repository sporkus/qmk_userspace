#pragma once

#include "quantum/keycodes.h"
#include "layout.h"

// This file should not be called [keyboard].h as it will cause conflict with qmk
// layout to be used in [keyboard].json | exact layout name defined in keyboards/[keyboard]/info.json
#define LAYOUT_split_4space_w(...)        LAYOUT_split_4space(__VA_ARGS__)

// layers
#define V45_BASE \
           KC_TAB,             LEFT_ROW1(_BASE),       RIGHT_ROW1(_BASE),   KC_BSPC, KC_MUTE, \
          NAV_ESC,       HRML(LEFT_ROW2(_BASE)), HRMR(RIGHT_ROW2(_BASE)),   KC_COLN,          \
          KC_LSFT,             LEFT_ROW3(_BASE),       RIGHT_ROW3(_BASE),   KC_RSFT,          \
 KC_NO,     KC_NO,            LEFT_THUMB(_BASE),      RIGHT_THUMB(_BASE),     KC_NO,   KC_NO

#define V45_NUM \
           _______,             LEFT_ROW1(_NUM),        RIGHT_ROW1(_NUM),   _______, _______, \
           _______,             LEFT_ROW2(_NUM),        RIGHT_ROW2(_NUM),   _______,          \
           _______,             LEFT_ROW3(_NUM),        RIGHT_ROW3(_NUM),   _______,          \
 _______,  _______,            LEFT_THUMB(_NUM),       RIGHT_THUMB(_NUM),   _______, _______

#define V45_NAV \
           GUI_TAB,             LEFT_ROW1(_NAV),        RIGHT_ROW1(_NAV),   _______, _______, \
           _______,             LEFT_ROW2(_NAV),        RIGHT_ROW2(_NAV),   _______,          \
           CW_TOGG,             LEFT_ROW3(_NAV),        RIGHT_ROW3(_NAV),   _______,          \
  _______, _______,            LEFT_THUMB(_NAV),       RIGHT_THUMB(_NAV),   _______, _______

#define V45_FN \
           _______,              LEFT_ROW1(_FN),         RIGHT_ROW1(_FN),   _______, _______, \
           _______,              LEFT_ROW2(_FN),         RIGHT_ROW2(_FN),   _______,          \
           _______,              LEFT_ROW3(_FN),         RIGHT_ROW3(_FN),   _______,          \
  _______, _______,             LEFT_THUMB(_FN),        RIGHT_THUMB(_FN),   _______, _______
