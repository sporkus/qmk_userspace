#pragma once

#include "quantum/keycodes.h"
#include "layout.h"

// This file should not be called [keyboard].h as it will cause conflict with qmk
// layout to be used in [keyboard].json | exact layout name defined in keyboards/[keyboard]/info.json
#define LAYOUT_knob_w(...)        LAYOUT_knob(__VA_ARGS__)

// layers
#define BASE_LAYER \
        LEFT_ROW1(_BASE),           KC_MUTE,             RIGHT_ROW1(_BASE),       \
        HRML(LEFT_ROW2(_BASE)),                          HRMR(RIGHT_ROW2(_BASE)), \
        LEFT_ROW3(_BASE),                                RIGHT_ROW3(_BASE),       \
        LEFT_THUMB(_BASE),                               RIGHT_THUMB(_BASE)

#define NUM_LAYER \
        LEFT_ROW1(_NUM),      _______,     RIGHT_ROW1(_NUM),    \
        LEFT_ROW2(_NUM),                   RIGHT_ROW2(_NUM),    \
        LEFT_ROW3(_NUM),                   RIGHT_ROW3(_NUM),    \
        LEFT_THUMB(_NUM),                  RIGHT_THUMB(_NUM)


#define NAV_LAYER \
        LEFT_ROW1(_NAV),      _______,     RIGHT_ROW1(_NAV),    \
        LEFT_ROW2(_NAV),                   RIGHT_ROW2(_NAV),    \
        LEFT_ROW3(_NAV),                   RIGHT_ROW3(_NAV),    \
        LEFT_THUMB(_NAV),                  RIGHT_THUMB(_NAV)


#define FN_LAYER \
        LEFT_ROW1(_FN),      _______,     RIGHT_ROW1(_FN),    \
        LEFT_ROW2(_FN),                   RIGHT_ROW2(_FN),    \
        LEFT_ROW3(_FN),                   RIGHT_ROW3(_FN),    \
        LEFT_THUMB(_FN),                  RIGHT_THUMB(_FN)
