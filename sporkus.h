// Copyright @sporkus
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "action.h"
#include "keycodes.h"
#include "os_detection.h"
#include "features/global_quick_tap.h"
#include "features/swapper.h"

#pragma once
#include QMK_KEYBOARD_H

/* // Mod-tap decision helper macros */
/* #define IS_TYPING() (timer_elapsed_fast(tap_timer) < TAPPING_TERM) */
#define IS_HOMEROW(r) (r->event.key.row == 1)

#ifdef COMBO_ENABLE
#	include "combos.h"
#endif

#ifdef KEY_OVERRIDE_ENABLE
#	include "key_overrides.h"
#endif

