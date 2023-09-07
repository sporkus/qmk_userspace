// Copyright @sporkus
// SPDX-License-Identifier: GPL-2.0+

// Core 34-key maps
// Individaul keyboard keymap are defined in [keyboard]_layout.h

#pragma once
#include "quantum/keycodes.h"
#include "quantum/quantum_keycodes.h"

enum custom_keycodes {
  NUMWORD = SAFE_RANGE,
  ALT_TAB,
  GUI_TAB,
  KB_SAFE_RANGE  //use "KB_SAFE_RANGE" for keyboard specific codes
};

// Mod tap macros
#define HM_A	LSFT_T(KC_A)
#define HM_S	LALT_T(KC_S)
#define HM_D	LGUI_T(KC_D)
#define HM_F	LCTL_T(KC_F)
#define HM_J	LCTL_T(KC_J)
#define HM_K	LGUI_T(KC_K)
#define HM_L	LALT_T(KC_L)
#define HM_QUOT	LSFT_T(KC_QUOT)
#define SFT_ENT RSFT_T(KC_ENT)
#define SFT_Z    LSFT_T(KC_Z)
#define SFT_SLSH RSFT_T(KC_SLSH)

#define HRML(halfrow) _HRML(halfrow)
#define HRMR(halfrow) _HRMR(halfrow)
#define _HRML(k1, k2, k3, k4, k5)     LSFT_T(k1), LALT_T(k2), LGUI_T(k3), LCTL_T(k4),         k5
#define _HRMR(k1, k2, k3, k4, k5)             k1, LCTL_T(k2), LGUI_T(k3), LALT_T(k4), LSFT_T(k5)
#define BRML(halfrow) _BRML(halfrow)
#define BRMR(halfrow) _BRMR(halfrow)
#define _BRML(k1, k2, k3, k4, k5)     LSFT_T(k1), k2, k3, k4, k5
#define _BRMR(k1, k2, k3, k4, k5)             k1, k2, k3, k4, RSFT_T(k5)

// Navigation shortcuts
#define SA_UP S(A(KC_UP))
#define SA_DN S(A(KC_DOWN))
#define GUI_GRV G(KC_GRV)

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
	                           KC_RCTL, NUM_SPC,     NAV_SPC, NUM_SPC

#define _NUM \
    FN_TILD, _______, _______, KC_SCLN, _______,     KC_EQL,  KC_7,    KC_8,    KC_9,    KC_0,    \
    KC_GRV,  _______, _______, KC_COLN, _______,     KC_MINS, KC_4,    KC_5,    KC_6,    _______, \
    _______, _______, _______, _______, _______,     KC_BSLS, KC_1,    KC_2,    KC_3,    _______, \
                               _______, _______,     KC_0,    KC_DOT

#define _NAV \
    GUI_GRV, _______, _______, _______, _______,     _______, _______, KC_BSPC, _______, _______, \
    _______, _______, _______, _______, _______,     KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, \
    CW_TOGG, _______, _______, _______, _______,     _______, KC_PGDN, KC_PGUP, _______, _______, \
                               _______, KC_ENT,      KC_TAB,  _______

#define _FN \
    _______, _______, _______, _______, _______,     _______, KC_F7,   KC_F8,   KC_F9,  KC_F10,  \
    _______, _______, _______, _______, _______,     _______, KC_F4,   KC_F5,   KC_F6,  KC_F11,  \
    _______, _______, _______, _______, _______,     _______, KC_F1,   KC_F2,   KC_F3,  KC_F12,  \
                               _______, _______,     _______, _______



// ------------------------------------------------------------------------------------------------------
// Helper to extract keycodes from layers
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

