/* Copyright 2023 sporkus
 * Copyright 2023 Cipulot
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

#include <math.h>
#include "ec_switch_matrix.h"
#include "analog.h"
#include "atomic_util.h"
#include "print.h"
#include "raw_hid.h"

/* Pin and port array */
const uint32_t row_pins[]     = EC_MATRIX_ROW_PINS;
const uint8_t  col_channels[] = EC_MATRIX_COL_CHANNELS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS;
const uint32_t mux_en_pins[] =  MUX_EN_PINS;

static adc_mux adcMux;
static uint16_t ecsm_sw_value[EC_MATRIX_ROWS][EC_MATRIX_COLS];

static ecsm_threshold_t ecsm_thresholds[EC_MATRIX_ROWS][EC_MATRIX_COLS];
static int16_t ecsm_tuning_data[EC_MATRIX_ROWS][EC_MATRIX_COLS];
static uint32_t ecsm_is_tuning = 1e5; // Tunes ec config until this counter reaches 0

bool ecsm_bottoming_cal_active = false;
bool ecsm_tui_active = false;

static uint32_t ec_hid_last_keepalive = 0;
static bool     ec_hid_was_connected  = false;
#define EC_HID_TIMEOUT_MS 5000

void ec_hid_keepalive(void) {
    ec_hid_last_keepalive = timer_read32();
}

static void ec_hid_task(void) {
    bool now_connected = ec_hid_last_keepalive != 0
                         && timer_elapsed32(ec_hid_last_keepalive) < EC_HID_TIMEOUT_MS;
    if (now_connected && !ec_hid_was_connected) {
        ec_hid_was_connected = true;
        if (!ecsm_tui_active) ecsm_tui_toggle();
    } else if (!now_connected && ec_hid_was_connected) {
        ec_hid_was_connected = false;
        if (ecsm_tui_active) ecsm_tui_toggle();
    }
}
static uint16_t ecsm_tui_scan_count = 0;
static uint16_t ecsm_tui_cfg_count = 0;
// Staggered config dump: -1 = idle/streaming, 0 = EC_CFG, 1..ROWS = EC_IDLE, ROWS+1..2*ROWS = EC_BOTTOM
static int8_t  ecsm_tui_dump_line = -1;
static uint8_t ecsm_tui_adc_row = 0;
static uint32_t ecsm_cal_tune_remaining = 0;
#define CAL_TUNE_CYCLES 3000  // baseline tuning duration at start of cal mode (~3s at 1kHz)
static bool     ecsm_pressing[EC_MATRIX_ROWS][EC_MATRIX_COLS];     // key currently above cal threshold
static uint16_t ecsm_reported_max[EC_MATRIX_ROWS][EC_MATRIX_COLS]; // last printed max per key
static bool ecsm_cal_saved_debug;
static bool ecsm_cal_saved_debug_kb;

/* fancy printing */
const char* red = "\x1b[31m";
const char* reset = "\x1b[0m";

/* --- structured logging for ec_calibration TUI --- */

// Send a string as 32-byte raw HID packets so the HTML tool can read via Raw HID.
static void ec_hid_print(const char *str) {
    uint8_t pkt[32];
    uint16_t len = strlen(str);
    uint16_t off = 0;
    while (off < len) {
        uint8_t chunk = (len - off) < 32 ? (len - off) : 32;
        memset(pkt, 0, 32);
        memcpy(pkt, str + off, chunk);
        raw_hid_send(pkt, 32);
        off += chunk;
    }
}

// Emit one line of the config dump per call. line 0 = EC_CFG, 1..ROWS = EC_IDLE, ROWS+1..2*ROWS = EC_BOTTOM.
// Returns true when the dump is complete.
static bool ecsm_emit_dump_line(int8_t line) {
    char buf[128];
    if (line == 0) {
        snprintf(buf, sizeof(buf), "EC_CFG:rows=%d,cols=%d,act=%d,rel=%d,min_travel=%d,default_bottom=%d,configured=%d,bottoming_cal=%d,gamma=%d\n",
            EC_MATRIX_ROWS, EC_MATRIX_COLS,
            ecsm_config.actuation_offset, ecsm_config.release_offset,
            CALIBRATION_MIN_TRAVEL, DEFAULT_BOTTOM_ADC,
            ecsm_config.configured ? 1 : 0,
            ecsm_config.bottoming_configured ? 1 : 0,
            (int)(TRAVEL_CURVE_GAMMA * 100));
    } else if (line <= EC_MATRIX_ROWS) {
        int r = line - 1;
        int pos = snprintf(buf, sizeof(buf), "EC_IDLE:%d:", r);
        for (int j = 0; j < EC_MATRIX_COLS; j++)
            pos += snprintf(buf + pos, sizeof(buf) - pos, j ? ",%d" : "%d", ecsm_tuning_data[r][j]);
        snprintf(buf + pos, sizeof(buf) - pos, "\n");
    } else if (line <= 2 * EC_MATRIX_ROWS) {
        int r = line - EC_MATRIX_ROWS - 1;
        int pos = snprintf(buf, sizeof(buf), "EC_BOTTOM:%d:", r);
        for (int j = 0; j < EC_MATRIX_COLS; j++)
            pos += snprintf(buf + pos, sizeof(buf) - pos, j ? ",%d" : "%d", ecsm_config.bottoming[r][j]);
        snprintf(buf + pos, sizeof(buf) - pos, "\n");
    } else {
        return true; // done
    }
    uprintf("%s", buf);
    ec_hid_print(buf);
    return false;
}

void ecsm_tui_toggle(void) {
    ecsm_tui_active = !ecsm_tui_active;
    if (ecsm_tui_active) {
        uprintln("EC TUI mode started");
        uprintf("EC_EVENT:tuiStarted\n");
        ec_hid_print("EC_EVENT:tuiStarted\n");
        ecsm_tui_dump_line = 0;  // queue config dump, emitted one line per scan cycle
        ecsm_tui_adc_row = 0;
    } else {
        ecsm_tui_dump_line = -1;
        uprintf("EC_EVENT:tuiStopped\n");
        ec_hid_print("EC_EVENT:tuiStopped\n");
    }
}

static inline void discharge_capacitor(void) {
    writePinLow(DISCHARGE_PIN);
}

static inline void charge_capacitor(uint8_t row) {
    writePinHigh(DISCHARGE_PIN); // Blocks discharge route
    writePinHigh(row_pins[row]); // Send signal to row
}

static inline void disable_mux(uint8_t i) {
    writePinHigh(mux_en_pins[i]);
}

static inline void disable_mux_all(void) {
    for (int i = 0; i < 2; i++) {
        disable_mux(i);
    }
}

static inline void enable_mux(uint8_t i) {
    writePinLow(mux_en_pins[i]);
}

static inline void select_col(uint8_t col) {
    uint8_t ch = col_channels[col];
    uint8_t active_mux = (ch & 8) ? 1 : 0;

    disable_mux(!active_mux);
    writePin(mux_sel_pins[0], ch & 1);
    writePin(mux_sel_pins[1], ch & 2);
    writePin(mux_sel_pins[2], ch & 4);
    enable_mux(active_mux);
}

/// @brief hardware initialization for row pins
static inline void init_row(void) {
    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        setPinOutput(row_pins[i]);
        writePinLow(row_pins[i]);
    }
}

/// @brief hardware initialization for mux
static inline void init_mux(void) {
    for (int i = 0; i < 2; i++) {
        setPinOutput(mux_en_pins[i]);
    }

    for (int idx = 0; idx < 3; idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

void ecsm_config_init(void) {
    eeconfig_read_kb_datablock(&ecsm_config);

    // Clamp stored offsets — catches corrupted values from pre-percentage firmware
    if (ecsm_config.actuation_offset > 85 || ecsm_config.release_offset > 85
            || ecsm_config.actuation_offset < 15 || ecsm_config.release_offset < 15) {
        uprintf("EC offsets out of range (%d, %d), resetting to defaults\n",
                ecsm_config.actuation_offset, ecsm_config.release_offset);
        ecsm_config.actuation_offset = ACTUATION_DEPTH;
        ecsm_config.release_offset   = RELEASE_DEPTH;
    }

    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            if (! ecsm_config.configured) {
                // fallback to default values
                ecsm_config.actuation_offset = ACTUATION_DEPTH;
                ecsm_config.release_offset = RELEASE_DEPTH;
                ecsm_config.idle[i][j] = DEFAULT_IDLE;
            }
            ecsm_tuning_data[i][j] = ecsm_config.idle[i][j];
        }
    }
    ecsm_update_thresholds();
}

void ecsm_config_update(void) {
    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            ecsm_config.idle[i][j] = ecsm_tuning_data[i][j];
        }
    }

    uprintf("Writing current actuation points to presistent storage\n");
    eeconfig_update_kb_datablock(&ecsm_config);
    ecsm_print_debug();
    ecsm_tui_dump_line = 0;
}

void ecsm_eeprom_clear(void) {
    uprintln("\nClearing EC config");
    ecsm_config.configured = 0;
    ecsm_config.bottoming_configured = false;
    ecsm_config.actuation_offset = ACTUATION_DEPTH;
    ecsm_config.release_offset = RELEASE_DEPTH;
    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            ecsm_config.idle[i][j] = 0;
            ecsm_config.bottoming[i][j] = 0;
        }
    }
    eeconfig_update_kb_datablock(&ecsm_config);
    ecsm_is_tuning = 1e5;
}

void ecsm_ap_inc(void) {
    int16_t max_offset = 85;
    int16_t step = 5;
    uprintf("\nIncreasing actuation depth (less sensitive)\n");
    int16_t offset_diff = ecsm_config.actuation_offset - ecsm_config.release_offset;
    ecsm_config.actuation_offset += step;
    ecsm_config.release_offset += step;

    if (ecsm_config.actuation_offset > max_offset || ecsm_config.release_offset > max_offset) {
        uprintf("\nActuation point at maximum\n");
        ecsm_config.actuation_offset = max_offset;
        ecsm_config.release_offset = max_offset;

        if (offset_diff > 0) {
            ecsm_config.release_offset = ecsm_config.actuation_offset - offset_diff;
        } else {
            ecsm_config.actuation_offset = ecsm_config.release_offset - offset_diff;
        }
    }

    ecsm_update_thresholds();
    ecsm_config_update();
}

void ecsm_ap_dec(void) {
    int16_t min_offset = 15;
    int16_t step = 5;
    uprintf("\nDecreasing actuation depth (more sensitive)\n");
    int16_t offset_diff = ecsm_config.actuation_offset - ecsm_config.release_offset;
    ecsm_config.actuation_offset -= step;
    ecsm_config.release_offset -= step;

    if (ecsm_config.actuation_offset < min_offset || ecsm_config.release_offset < min_offset) {
        uprintf("\nActuation point at minimum\n");
        ecsm_config.actuation_offset = min_offset;
        ecsm_config.release_offset = min_offset;

        if (offset_diff > 0) {
            ecsm_config.actuation_offset = ecsm_config.release_offset + offset_diff;
        } else {
            ecsm_config.release_offset = ecsm_config.actuation_offset + offset_diff;
        }
    }

    ecsm_update_thresholds();
    ecsm_config_update();
}


void ecsm_init(void) {
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);
    //Dummy call to make sure that adcStart() has been called in the appropriate state
    adc_read(adcMux);

    // Initialize discharge pin as discharge mode
    writePinLow(DISCHARGE_PIN);
    setPinOutputOpenDrain(DISCHARGE_PIN);

    init_row();
    init_mux();
    ecsm_config_init();
}

void ecsm_update_tuning_data(int16_t new_value, uint8_t row, uint8_t col) {
    int16_t idle = ecsm_tuning_data[row][col];
    if (new_value < idle + (DEFAULT_BOTTOM_ADC - idle) * ACTUATION_DEPTH / 100) {
        float curr = ecsm_tuning_data[row][col];
        float adjusted = curr + ((float)new_value - curr) * 0.02;
        ecsm_tuning_data[row][col] = round(adjusted);
    }
}

void ecsm_update_thresholds(void) {
    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            int16_t idle          = ecsm_tuning_data[i][j];
            int16_t default_travel = DEFAULT_BOTTOM_ADC - idle;
            int16_t travel        = (int16_t)ecsm_config.bottoming[i][j] - idle;
            int16_t min_travel    = default_travel * CALIBRATION_MIN_TRAVEL / 100;

            if (!ecsm_config.bottoming_configured || travel < min_travel) {
                travel = default_travel;
            }

            // Offsets are percentages (0-100) of travel, corrected for the ADC curve
            float gamma_inv = 1.0f / TRAVEL_CURVE_GAMMA;
            ecsm_thresholds[i][j].actuation = idle + (int16_t)(powf(ecsm_config.actuation_offset / 100.0f, gamma_inv) * travel);
            ecsm_thresholds[i][j].release   = idle + (int16_t)(powf(ecsm_config.release_offset   / 100.0f, gamma_inv) * travel);
        }
    }
}

uint16_t ecsm_readkey_raw(uint8_t row, uint8_t col) {
    uint16_t sw_value = 0;

    select_col(col);
    // Set strobe pins to idle state
    writePinLow(row_pins[row]);
    ATOMIC_BLOCK_FORCEON {
        charge_capacitor(row);
        __asm__ __volatile__("nop;nop;nop;\n\t");
        sw_value = adc_read(adcMux);
    }
    // reset sensor
    writePinLow(row_pins[row]);
    discharge_capacitor();
    return sw_value;
}

/// Comparing ADC reading to threshold to update press/release state of key
bool ecsm_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value) {
    bool current_state = (*current_row >> col) & 1;

    // Press to release
    if (current_state && sw_value < ecsm_thresholds[row][col].release) {
        *current_row &= ~(1 << col);
        return true;
    }

    // Release to press
    if (!current_state && sw_value > ecsm_thresholds[row][col].actuation) {
        *current_row |= (1 << col);
        return true;
    }

    return false;
}

void ecsm_print_matrix(matrix_row_t current_matrix[]) {
    uprintln();
    uprintf("[ADC]    ");
    for (int j = 0; j < EC_MATRIX_COLS; j++) uprintf("C%-3d ", j);
    uprintln();

    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        uprintf("  R%d:  ", i);
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            bool key_pressed = (current_matrix[i] >> j) & 1;
            if (key_pressed) {
                uprintf("%s%4u%s ", red, ecsm_sw_value[i][j], reset);
            } else {
                uprintf("%4u ", ecsm_sw_value[i][j]);
            }
        }
        uprintln();
    }

    uprintf("[Margin] ");
    for (int j = 0; j < EC_MATRIX_COLS; j++) uprintf("C%-3d ", j);
    uprintln();

    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        uprintf("  R%d:  ", i);
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            int16_t margin = (int16_t)ecsm_sw_value[i][j] - ecsm_tuning_data[i][j];
            uprintf("%+4d ", margin);
        }
        uprintln();
    }
    uprintln();
}

void ecsm_print_debug(void) {
    uprintln();
    uprintf("Actuation/release depth: %d, %d  [%s]  bottoming: %s\n",
            ecsm_config.actuation_offset, ecsm_config.release_offset,
            ecsm_config.bottoming_configured ? "% of travel" : "raw ADC units",
            ecsm_config.bottoming_configured ? "YES" : "NO");

    uprintf("[Idle]   ");
    for (int j = 0; j < EC_MATRIX_COLS; j++) uprintf("C%-3d ", j);
    uprintln();

    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        uprintf("  R%d:  ", i);
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            uprintf("%4u ", ecsm_tuning_data[i][j]);
        }
        uprintln();
    }

    if (ecsm_config.bottoming_configured) {
        uprintf("\n[Bottom] ");
        for (int j = 0; j < EC_MATRIX_COLS; j++) uprintf("C%-3d ", j);
        uprintln();

        for (int i = 0; i < EC_MATRIX_ROWS; i++) {
            uprintf("  R%d:  ", i);
            for (int j = 0; j < EC_MATRIX_COLS; j++) {
                uprintf("%4u ", ecsm_config.bottoming[i][j]);
            }
            uprintln();
        }
    }

    uprintf("\n[Act]    ");
    for (int j = 0; j < EC_MATRIX_COLS; j++) uprintf("C%-3d ", j);
    uprintln();

    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        uprintf("  R%d:  ", i);
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            uprintf("%4u ", ecsm_thresholds[i][j].actuation);
        }
        uprintln();
    }
    uprintln();
}

static void ecsm_cal_begin_bottoming_phase(void) {
    for (int i = 0; i < EC_MATRIX_ROWS; i++) {
        for (int j = 0; j < EC_MATRIX_COLS; j++) {
            ecsm_config.bottoming[i][j] = ecsm_tuning_data[i][j];
            ecsm_reported_max[i][j]     = ecsm_tuning_data[i][j];
        }
    }
    memset(ecsm_pressing, 0, sizeof(ecsm_pressing));
    uprintf("EC_EVENT:calTuningDone\n");
    ec_hid_print("EC_EVENT:calTuningDone\n");
    ecsm_tui_dump_line = 0;
    uprintln("Baseline tuning done. Bottom all keys then press EC_CAL again.");
}

static void ecsm_bottoming_cal_start(void) {
    if (!ecsm_tui_active) {
      ecsm_tui_toggle();
    }
    if (!ecsm_config.configured) {
        uprintln("Bottoming calibration blocked: idle tuning not yet complete.");
        return;
    }
    ecsm_cal_saved_debug    = debug_config.enable;
    ecsm_cal_saved_debug_kb = debug_config.keyboard;
    debug_config.enable   = false;
    debug_config.keyboard = false;
    ecsm_bottoming_cal_active = true;
    ecsm_cal_tune_remaining = CAL_TUNE_CYCLES;
    uprintln("Calibration started: keep all fingers off keyboard for baseline tuning.");
    uprintf("EC_EVENT:calStarted\n");
    ec_hid_print("EC_EVENT:calStarted\n");
    ecsm_tui_dump_line = 0;
}

static void ecsm_bottoming_cal_save(void) {
    ecsm_bottoming_cal_active = false;
    debug_config.enable   = ecsm_cal_saved_debug;
    debug_config.keyboard = ecsm_cal_saved_debug_kb;
    ecsm_config.bottoming_configured = true;

    ecsm_update_thresholds();
    ecsm_config_update();
    uprintln("Bottoming calibration saved.");
    uprintf("EC_EVENT:calSaved\n");
    ec_hid_print("EC_EVENT:calSaved\n");
}

void ecsm_bottoming_cal_toggle(void) {
    if (ecsm_bottoming_cal_active) {
        ecsm_bottoming_cal_save();
    } else {
        ecsm_bottoming_cal_start();
    }
}

// Scan key values and update matrix state
bool ecsm_matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = false;

    for (int col = 0; col < EC_MATRIX_COLS; col++) {
        for (int row = 0; row < EC_MATRIX_ROWS; row++) {
            uint16_t adc = ecsm_readkey_raw(row, col);
            ecsm_sw_value[row][col] = adc;
            updated |= ecsm_update_key(&current_matrix[row], row, col, adc);

            if (! ecsm_config.configured) {
                if (ecsm_is_tuning > 0) {
                    ecsm_is_tuning--;
                }
                ecsm_update_tuning_data(adc, row, col);

                if (ecsm_is_tuning == 0) {
                    uprintln("EC config tuning completed");
                    ecsm_update_thresholds();
                    ecsm_config.configured = true;
                    ecsm_config_update();
                }
            }

            if (ecsm_bottoming_cal_active) {
                if (ecsm_cal_tune_remaining > 0) {
                    // Phase 1: retune baseline with hands off
                    ecsm_update_tuning_data(adc, row, col);
                } else {
                    // Phase 2: record per-key bottoming max
                    int16_t idle = ecsm_tuning_data[row][col];
                    bool above = adc > (uint16_t)(idle + (DEFAULT_BOTTOM_ADC - idle) * CALIBRATION_MIN_TRAVEL / 100);

                    if (adc > ecsm_config.bottoming[row][col])
                        ecsm_config.bottoming[row][col] = adc;

                    if (above && !ecsm_pressing[row][col]) {
                        ecsm_pressing[row][col] = true;
                    } else if (!above && ecsm_pressing[row][col]) {
                        ecsm_pressing[row][col] = false;
                        if (ecsm_config.bottoming[row][col] > ecsm_reported_max[row][col]) {
                            ecsm_reported_max[row][col] = ecsm_config.bottoming[row][col];
                            uprintf("  R%d,C%d bottomed: %u\n", row, col, ecsm_config.bottoming[row][col]);
                            uprintf("EC_KEY_BOTTOM:%d,%d:%u\n", row, col, ecsm_config.bottoming[row][col]);
                            char kbuf[32];
                            snprintf(kbuf, sizeof(kbuf), "EC_KEY_BOTTOM:%d,%d:%u\n", row, col, ecsm_config.bottoming[row][col]);
                            ec_hid_print(kbuf);
                        }
                    }
                }
            }
        }
    }

    // Advance phase 1 → phase 2 once tuning countdown completes
    if (ecsm_bottoming_cal_active && ecsm_cal_tune_remaining > 0) {
        if (--ecsm_cal_tune_remaining == 0) {
            ecsm_update_thresholds();
            ecsm_cal_begin_bottoming_phase();
        }
    }

    /* --- structured ADC streaming for ec_calibration TUI ---
     * One line emitted per 20-scan tick to avoid overflowing the HID console
     * buffer, which is especially tight on Vial builds with multiple HID interfaces. */
    if (ecsm_tui_active && ++ecsm_tui_scan_count >= 20) {
        ecsm_tui_scan_count = 0;
        if (ecsm_tui_dump_line >= 0) {
            // Config dump in progress: emit one line then advance
            if (ecsm_emit_dump_line(ecsm_tui_dump_line)) {
                ecsm_tui_dump_line = -1; // dump complete, resume ADC streaming
            } else {
                ecsm_tui_dump_line++;
            }
        } else {
            // Normal streaming: one ADC row per tick, rotating through all rows
            char abuf[128];
            int apos = snprintf(abuf, sizeof(abuf), "EC_ADC:%d:", ecsm_tui_adc_row);
            for (int c = 0; c < EC_MATRIX_COLS; c++)
                apos += snprintf(abuf + apos, sizeof(abuf) - apos, c ? ",%d" : "%d", ecsm_sw_value[ecsm_tui_adc_row][c]);
            snprintf(abuf + apos, sizeof(abuf) - apos, "\n");
            uprintf("%s", abuf);
            ec_hid_print(abuf);
            ecsm_tui_adc_row = (ecsm_tui_adc_row + 1) % EC_MATRIX_ROWS;

            if (++ecsm_tui_cfg_count >= 50) {
                ecsm_tui_cfg_count = 0;
                ecsm_tui_dump_line = 0; // queue periodic config refresh
            }
        }
    }

    ec_hid_task();
    return updated;
}
