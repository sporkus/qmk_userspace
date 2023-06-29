#pragma once

#define CAPS_WORD_IDLE_TIMEOUT 3000
#define USB_POLLING_INTERVAL_MS 1

// Tap-hold settings
#define TAPPING_TERM 280
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM TAPPING_TERM
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define GLOBAL_QUICK_TAP_MS_PER_KEY

// Autoshift
#define AUTO_SHIFT_TIMEOUT 150
#define NO_AUTO_SHIFT_ALPHA
#define NO_AUTO_SHIFT_SPECIAL

#ifdef COMBO_ENABLE
#	define EXTRA_SHORT_COMBOS
#	define COMBO_SHOULD_TRIGGER
#	define COMBO_ONLY_FROM_LAYER 0
#endif

// Layout macros
#ifndef __ASSEMBLER__

#ifdef KEYBOARD_projectcain_vault45
# include "keymaps/vault45_layout.h"
#endif

#ifdef KEYBOARD_mkh_studio_bully
# include "keymaps/bully_layout.h"
#endif

#ifdef KEYBOARD_sporkus_bully2040
# include "keymaps/bully2040_layout.h"
#endif

#ifdef KEYBOARD_sporkus_le_chiffre_32
# include "keymaps/le_chiffre_32_layout.h"
#endif

#ifdef KEYBOARD_weteor_kong_mx
# include "keymaps/kong_mx.h"
#endif

#endif

