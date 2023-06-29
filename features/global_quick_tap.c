#include "global_quick_tap.h"

static struct {
    uint16_t keycode;
    bool disabled;
    bool key_registered;
} gqt_state = {0, false, false};

void reset_global_quick_tap_state(void) {
    gqt_state.keycode = 0;
    gqt_state.disabled = false;
    gqt_state.key_registered = false;
}

static uint16_t prev_press_time = 0;

bool process_global_quick_tap(uint16_t keycode, keyrecord_t* record) {
    if (record->event.pressed) {
        /* Any key down. Recording key press times */
        uint16_t time_diff = timer_elapsed_fast(prev_press_time);
        prev_press_time = timer_read_fast();

        if (is_caps_word_on()) {
            return true;
        }

        if (!gqt_state.disabled) {
            uint16_t quick_tap_ms = get_global_quick_tap_ms(keycode, record);
            if (quick_tap_ms > 0) {
                /* #ifdef CONSOLE_ENABLE */
                /* uprintf("Global-quick-tap: Last key press was %dms ago, global_quick_tap is %dms. ", time_diff, quick_tap_ms); */
                /* #endif */

                if (quick_tap_ms > time_diff) {
                    #ifdef CONSOLE_ENABLE
                    uprintf("Disabling hold-tap for 0x%04X; ", keycode);
                    uprintf("registering 0x%04X\n", QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
                    #endif
                    register_code16(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
                    gqt_state.key_registered = true;
                    gqt_state.keycode = keycode;
                    return false;
                } else {
                    #ifdef CONSOLE_ENABLE
                    uprintf("Hold-taps and mod combos are allowed for key 0x%04X\n", keycode);
                    #endif
                    gqt_state.disabled = true;
                    gqt_state.keycode = keycode;
                }
            }
        }
    } else {
        /* key up. Cleaning up global_quick_tap_state */
        if (keycode == gqt_state.keycode) {
            #ifdef CONSOLE_ENABLE
            uprintf("Global-quick-tap: 0x%04X key released. ", keycode);
            #endif
            if (gqt_state.disabled) {
                #ifdef CONSOLE_ENABLE
                uprintf("Resuming global_quick_tap processing\n");
                #endif
            } else if (gqt_state.key_registered) {
                #ifdef CONSOLE_ENABLE
                uprintf("Unregistering key\n");
                #endif
                unregister_code(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
            }
            reset_global_quick_tap_state();
        }
    }

    return true;
};

__attribute__((weak))
uint16_t get_global_quick_tap_ms(uint16_t keycode, keyrecord_t* record) {
    if (IS_QK_MOD_TAP(keycode)) {
        uint16_t tap_keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        switch (tap_keycode) {
            /* Example: alphanumerics and punctuations will not be considered for hold-tap if the last key press was less than GLOBAL_QUICK_TAP_MS ago */
            case KC_A ... KC_0:
            case KC_MINUS ... KC_SLASH:
                return 200;
        }
    }

    /* if (IS_QK_LAYER_TAP(keycode)) { */
    /*     uint16_t tap_keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode); */
    /*     switch (tap_keycode) { */
    /*         case KC_SPC: */
    /*             return GLOBAL_QUICK_TAP_MS; */
    /*     } */
    /* } */

    return 0;
}

