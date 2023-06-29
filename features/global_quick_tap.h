# pragma once

#include "quantum.h"

#ifdef __cplusplus
extern "C" {
#endif

/* user configuration function for global quick tap time by keycode */
/* example: */
/* uint16_t get_global_quick_tap_ms(uint16_t keycode) { */
/*     switch (keycode) { */
/*         case CTL_T(KC_A): */
/*             return 200; */
/*         default: */
/*             return 0; */
/*     } */
/* } */
uint16_t get_global_quick_tap_ms(uint16_t keycode, keyrecord_t* record);

/*
  - Global-quick-tap applies to any keycode that has the quick tap term defined by the user.
  - If a global-quick-tap enabled key is pressed within its defined time, its tap keycode
  will be registered immediately. The tap keycode can be held without double tapping if
  the hold-tap key was overriden in this manner.
  - If a global-quick-tap enabled key is allowed to be processed as hold-tap key, global-quick-tap
  is disabled until the same key is released. This allows home row mods keys to combine.
  -------------------------------------------------------------------------------------------
  returns true: normal hold-tap key processing
  returns false: send tap immediately on key down if global quick tap applies to current keycode

  usage example:
  bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // applies before achordion
    if (!process_global_quick_tap(keycode, record)) {return false; }
  }
 */
bool process_global_quick_tap(uint16_t keycode, keyrecord_t* record);

void reset_global_quick_tap_state(void);
