#pragma once

#define CAPS_WORD_IDLE_TIMEOUT 2000
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD
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

