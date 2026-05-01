// Copyright @sporkus
// SPDX-License-Identifier: GPL-2.0+

// Core 34-key maps
// Individaul keyboard keymap are defined in [keyboard]_layout.h

#pragma once
#include "quantum/keycodes.h"
#include "quantum/quantum_keycodes.h"

enum custom_keycodes {
  NUMWORD = SAFE_RANGE,
  // ALT_TAB,
  // GUI_TAB,
  KB_SAFE_RANGE  //use "KB_SAFE_RANGE" for keyboard specific codes
};

// Homerow mod applicators — numbered L5..L1 R1..R5 from left to right
//L5 L4 L3 L2 L1 R1 R2 R3 R4 R5
#define HML5(k) k
#define HML4(k) LALT_T(k)
#define HML3(k) LCTL_T(k)
#define HML2(k) LSFT_T(k)
#define HML1(k) LGUI_T(k)
#define HMR1(k) RGUI_T(k)
#define HMR2(k) RSFT_T(k)
#define HMR3(k) RCTL_T(k)
#define HMR4(k) RALT_T(k)
#define HMR5(k) k

// Layers
enum layers {BASE, NUM, NAV, FN};
#define NAV_SPC LT(NAV, KC_SPC)
#define NAV_ESC LT(NAV, KC_ESC)
#define NUM_SPC LT(NUM, KC_SPC)
#define FN_TILD LT(FN,  KC_TILD)  //needs user processing

#define  XXX KC_NO

// Default 3x5_2 split layout
#define _BASE \
	KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    \
	KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, \
	KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, \
	                           KC_RCTL, MO(NUM),     NAV_SPC, NUM_SPC

#define _NUM \
    FN_TILD, S(KC_9), S(KC_8), S(KC_7), _______,     KC_EQL,  KC_7,    KC_8,    KC_9,    KC_0,    \
    KC_GRV,  S(KC_6), S(KC_5), S(KC_4), _______,     KC_MINS, KC_4,    KC_5,    KC_6,    _______, \
    _______, S(KC_3), S(KC_2), S(KC_1), _______,     KC_GRV,  KC_1,    KC_2,    KC_3,    KC_BSLS, \
                               _______, _______,     _______, _______

#define _NAV \
    _______, S(KC_9), S(KC_8), S(KC_7), _______,     _______, _______, KC_ESC,  _______, KC_BSPC, \
    _______, S(KC_6), S(KC_5), S(KC_4), _______,     KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, \
    CW_TOGG, S(KC_3), S(KC_2), S(KC_1), _______,     _______, KC_PGDN, KC_PGUP, _______, _______, \
                               _______, KC_ENT,      _______,  _______

#define _FN \
    _______, _______, _______, _______, _______,     _______, KC_F7,   KC_F8,   KC_F9,  KC_F10,  \
    _______, _______, _______, _______, _______,     _______, KC_F4,   KC_F5,   KC_F6,  KC_F11,  \
    _______, _______, _______, _______, _______,     _______, KC_F1,   KC_F2,   KC_F3,  KC_F12,  \
                               _______, _______,     _______, _______




// ------------------------------------------------------------------------------------------------------
// Helper to extract keycodes from layers
// Mod tap macros — for use in combos.def and direct references
#define HM_A     HML5(KC_A)
#define HM_S     HML4(KC_S)
#define HM_D     HML3(KC_D)
#define HM_F     HML2(KC_F)
#define HM_G     HML1(KC_z)
#define HM_H     HMR1(KC_H)
#define HM_J     HMR2(KC_J)
#define HM_K     HMR3(KC_K)
#define HM_L     HMR4(KC_L)
#define KC_QUOT  HMR5(KC_QUOT)
#define SFT_Z    HML5(KC_Z)
#define SFT_SLSH HMR5(KC_SLSH)

#define HRML(halfrow) _HRML(halfrow)
#define HRMR(halfrow) _HRMR(halfrow)
#define _HRML(k1, k2, k3, k4, k5)  HML5(k1), HML4(k2), HML3(k3), HML2(k4), HML1(k5)
#define _HRMR(k1, k2, k3, k4, k5)  HMR1(k1), HMR2(k2), HMR3(k3), HMR4(k4), HMR5(k5)
#define BRML(halfrow) _BRML(halfrow)
#define BRMR(halfrow) _BRMR(halfrow)
#define _BRML(k1, k2, k3, k4, k5)  HML5(k1), k2, k3, k4, k5
#define _BRMR(k1, k2, k3, k4, k5)          k1, k2, k3, k4, HMR5(k5)

#define LEFT_ROW1(layer)   _LEFT_ROW1(layer)
#define LEFT_ROW2(layer)   _LEFT_ROW2(layer)
#define LEFT_ROW3(layer)   _LEFT_ROW3(layer)
#define LEFT_THUMB(layer)  _LEFT_THUMB(layer)
#define RIGHT_ROW1(layer)  _RIGHT_ROW1(layer)
#define RIGHT_ROW2(layer)  _RIGHT_ROW2(layer)
#define RIGHT_ROW3(layer)  _RIGHT_ROW3(layer)
#define RIGHT_THUMB(layer) _RIGHT_THUMB(layer)
#define   _LEFT_ROW1(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) L1,  L2,  L3,  L4,  L5
#define   _LEFT_ROW2(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) L6,  L7,  L8,  L9, L10
#define   _LEFT_ROW3(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) L11, L12, L13, L14, L15
#define  _LEFT_THUMB(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) LT1, LT2
#define  _RIGHT_ROW1(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) R1,  R2,  R3,  R4,  R5
#define  _RIGHT_ROW2(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) R6,  R7,  R8,  R9, R10
#define  _RIGHT_ROW3(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) R11, R12, R13, R14, R15
#define _RIGHT_THUMB(L1, L2, L3, L4, L5, R1, R2, R3, R4, R5, L6, L7, L8, L9, L10, R6, R7, R8, R9, R10, L11, L12, L13, L14, L15, R11, R12, R13, R14, R15, LT1, LT2, RT1, RT2) RT1, RT2

#ifdef COMBO_ENABLE
#   include "combos.h"
#endif

#ifdef KEY_OVERRIDE_ENABLE
#   include "key_overrides.h"
#endif

