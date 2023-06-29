/* Copyright 2023 Foon W.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "num_word.h"

static uint16_t num_word_start_timer = 0;
static uint16_t num_word_released_timer = 0;
static bool is_num_word_on = false;
static int num_word_layer = 1;

void set_num_word_layer(int layer) {
    num_word_layer = layer;
}

bool is_num_word_enabled(void) {
    return is_num_word_on;
}

void reset_num_word_timeout() {
    num_word_released_timer = timer_read();
}

void enable_num_word(void) {
    if (is_num_word_on) return;
    is_num_word_on = true;
    layer_on(num_word_layer);
    num_word_start_timer = timer_read(); //tracks when num_word is pressed
}

void disable_num_word(void) {
    if (!is_num_word_on) return;
    is_num_word_on = false;
    layer_off(num_word_layer);
    num_word_released_timer = 0;
}

void toggle_num_word(void) {
    if (is_num_word_on) {
        disable_num_word();
    }
    else {
        enable_num_word();
    }
}

__attribute__ ((weak))
bool should_terminate_num_word(uint16_t keycode, const keyrecord_t *record) {
    // keep num_word on unless num_word key is released
    if (! num_word_released_timer) return false;

    switch (keycode) {
        // Keycodes which should not disable num word mode.
        case KC_1 ... KC_0:
        case KC_EQL:
        case KC_MINS:
        case KC_DOT:
        case KC_COMMA:
        // Numpad keycodes
        case KC_P1 ... KC_P0:
        case KC_PSLS ... KC_PPLS:
        case KC_PDOT:
        // Misc
        case KC_UNDS:
        case KC_BSPC:
            reset_num_word_timeout();
            return false;

        default:
            if (! record->event.pressed) {
                // terminate num_word on key release
                // so that keycodes can be properly unregistered
                return true;
            }
            return false;
    }
    return false;
}

void num_word_tasks(void) {
    #ifdef NUM_WORD_IDLE_TIMEOUT
    if (is_num_word_on && num_word_released_timer) {
        if (timer_elapsed(num_word_released_timer) > NUM_WORD_IDLE_TIMEOUT) {
            disable_num_word();
        }
    }
    #endif
}


bool process_record_num_word(uint16_t keycode, const keyrecord_t *record) {
    // Handle the custom keycodes that go with this feature
    if (keycode == NUMWORD) {
        if (record->event.pressed) {
            toggle_num_word();
        }
        else {
            if (timer_elapsed(num_word_start_timer) > TAPPING_TERM) {
                // If the user held the key longer than TAPPING_TERM,
                // consider it a hold, and disable the behavior on
                // key release.
                disable_num_word();
            } else {
                // num_word started by a quick tap
                num_word_released_timer = timer_read();
            }
        }
        return false;
    }

    // QMK to handle the event as usual if num_word is off
    if (!is_num_word_on) {
        return true;
    }

    // Get the base keycode of a mod or layer tap key
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            // Earlier return if this has not been considered tapped yet
            if (record->tap.count == 0)
                return true;
            keycode = keycode & 0xFF;
            break;
        default:
            break;
    }

    if (should_terminate_num_word(keycode, record)) {
        disable_num_word();
    }

    return true;
}

