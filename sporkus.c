#include "sporkus.h"

bool swapper_alt_tab_active = false;
bool swapper_gui_tab_active = false;

#ifdef HOLD_ON_OTHER_KEY_PRESS_PER_KEY
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case NAV_ESC:
        case NUM_SPC:
        case SFT_ENT:
        case SFT_Z:
        case SFT_SLSH:
            // Immediately select the hold action when another key is pressed.
            return true;
        default:
            // Do not select the hold action when another key is pressed.
            return false;
    }
}
#endif

#ifdef TAPPING_TERM_PER_KEY
static fast_timer_t tap_timer = 0;

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (timer_elapsed_fast(tap_timer) < TAPPING_TERM) {
        if (record->event.key.row == 1 || keycode == NAV_SPC) {
            return 300;
        }
    }
    return TAPPING_TERM;
}
#endif


#ifdef GLOBAL_QUICK_TAP_MS_PER_KEY
uint16_t get_global_quick_tap_ms(uint16_t keycode, keyrecord_t* record) {
    if (IS_QK_MOD_TAP(keycode)) {
        return IS_HOMEROW(record)? 250 : 1;
    }

    return 0;
}
#endif

// return false to interrupt normal processing
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    #ifdef CONSOLE_ENABLE
        uprintf("[%u, %u] kc: 0x%04X, pressed: %X, time: %5u, int: %u, tap.count: %u",
                record->event.key.row, record->event.key.col, keycode,  record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    #endif

    if (!process_global_quick_tap(keycode, record)) {return false; }

    // handling non-basic keycodes in hold-taps
    switch (keycode) {
        case FN_TILD:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_TILD);
                return false;
            }
    }

    update_swapper(
        &swapper_gui_tab_active, KC_LGUI, KC_TAB, GUI_TAB,
        keycode, record
    );

    update_swapper(
        &swapper_alt_tab_active, KC_LALT, KC_TAB, ALT_TAB,
        keycode, record
    );

    return true;
}


layer_state_t layer_state_set_user(layer_state_t state) {
#ifdef CONSOLE_ENABLE
    uprintf("layer: %u \n", get_highest_layer(state));
#endif
    return state;
}
