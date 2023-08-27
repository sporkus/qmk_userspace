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

#include "ec_switch_matrix.h"
#include "analog.h"
#include "wait.h"
#include "atomic_util.h"
#include "print.h"

/* Pin and port array */
const uint32_t row_pins[]     = MATRIX_ROW_PINS;
const uint8_t  col_channels[] = MATRIX_COL_CHANNELS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS;
const uint32_t mux_en_pins[] =  MUX_EN_PINS;

bool ecsm_update_tuning = false;
static adc_mux adcMux;
static uint16_t ecsm_sw_value[MATRIX_ROWS][MATRIX_COLS];
static ecsm_tune_data_t ecsm_tune_data[MATRIX_ROWS][MATRIX_COLS];

#ifdef ECSM_THRESHOLDS
static ecsm_threshold_t ecsm_threshold[MATRIX_ROWS][MATRIX_COLS] = ECSM_THRESHOLDS;
#else
static ecsm_threshold_t ecsm_threshold[MATRIX_ROWS][MATRIX_COLS];
#endif

/* fancy printing */
const char* red = "\x1b[31m";
const char* reset = "\x1b[0m";


static inline void discharge_capacitor(void) {
    writePinLow(DISCHARGE_PIN);
}

static inline void charge_capacitor(uint8_t row) {
    // Blocks discharge route
    writePinHigh(DISCHARGE_PIN);
    // Send signal to row
    writePinHigh(row_pins[row]);
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

static inline void init_row(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinOutput(row_pins[i]);
        writePinLow(row_pins[i]);
    }
}

static inline void init_mux(void) {
    for (int i = 0; i < 2; i++) {
        setPinOutput(mux_en_pins[i]);
    }

    for (int idx = 0; idx < 3; idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

/* Initialize the peripherals pins */
int ecsm_init(void) {
    // Default values, overwritten by VIA if enabled later
    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            #ifndef ECSM_THRESHOLDS
            ecsm_threshold[i][j].actuation = DEFAULT_ACTUATION_LEVEL;
            ecsm_threshold[i][j].release = DEFAULT_RELEASE_LEVEL;
            #endif
            ecsm_tune_data[i][j].low = DEFAULT_LOW_LEVEL;
            ecsm_tune_data[i][j].high = DEFAULT_ACTUATION_LEVEL;
        }
    }

    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);

    //Dummy call to make sure that adcStart() has been called in the appropriate state
    adc_read(adcMux);

    // Initialize discharge pin as discharge mode
    writePinLow(DISCHARGE_PIN);
    setPinOutputOpenDrain(DISCHARGE_PIN);

    init_row();
    init_mux();
    return 0;
}

void ecsm_update_tune_data(uint16_t new_value, uint8_t row, uint8_t col) {
    if (ecsm_update_tuning) {
        if (new_value > ecsm_tune_data[row][col].high && new_value > DEFAULT_HIGH_LEVEL) {
            // Updating max ADC reading
            ecsm_tune_data[row][col].high = (ecsm_tune_data[row][col].high * 0.5 + new_value * 0.5);
        } else if (new_value > DEFAULT_LOW_LEVEL && new_value < DEFAULT_RELEASE_LEVEL) {
            // Updating idle ADC reading
            ecsm_tune_data[row][col].low = (ecsm_tune_data[row][col].low * 0.9 + new_value * 0.1);
        }
    }
}

void ecsm_update_threshold(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            uint16_t low = ecsm_tune_data[i][j].low;
            uint16_t high = ecsm_tune_data[i][j].high;
            bool tune_data_changed = high > DEFAULT_HIGH_LEVEL && low > DEFAULT_LOW_LEVEL;

            if (tune_data_changed) {
                ecsm_threshold[i][j].actuation = low + ACTUATION_OFFSET;
                ecsm_threshold[i][j].release = low + RELEASE_OFFSET;
            }
        }
    }
}

// Read the capacitive sensor value
uint16_t ecsm_readkey_raw(uint8_t row, uint8_t col) {
    uint16_t sw_value = 0;

    select_col(col);
    // Set strobe pins to low state
    writePinLow(row_pins[row]);
    ATOMIC_BLOCK_FORCEON {
        charge_capacitor(row);
        __asm__ __volatile__("nop;nop;nop;\n\t");
        sw_value = adc_read(adcMux);
    }
    // reset sensor
    writePinLow(row_pins[row]);
    discharge_capacitor();
    ecsm_update_tune_data(sw_value, row, col);
    return sw_value;
}

// Update press/release state of key
bool ecsm_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value) {
    bool current_state = (*current_row >> col) & 1;

    // Press to release
    if (current_state && sw_value < ecsm_threshold[row][col].release) {
        *current_row &= ~(1 << col);
        return true;
    }

    // Release to press
    if (!current_state && sw_value > ecsm_threshold[row][col].actuation) {
        *current_row |= (1 << col);
        return true;
    }

    return false;
}

void ecsm_print_tuning(void) {
    uprintf("Threholds: \n");
    for (int row = MATRIX_ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (ecsm_tune_data[row][col].low > DEFAULT_LOW_LEVEL) {
                uprintf("ADC [%u, %u] current: %u; low: %u; high: %u, actuation: %u; release: %u\n",
                    row
                    , col
                    , ecsm_sw_value[row][col]
                    , ecsm_tune_data[row][col].low
                    , ecsm_tune_data[row][col].high
                    , ecsm_threshold[row][col].actuation
                    , ecsm_threshold[row][col].release
                );
            }
        }
    }
    print("--------------------------------------\n");
}

void ecsm_print_thresholds(matrix_row_t current_matrix[]) {
    // Prints ec matrix config block
    uprintf("\n#define ECSM_THRESHOLDS {\\\n");
    for (int i = 0; i < MATRIX_ROWS; i++) {
        uprintf("\t{");
        for (int j = 0; j < MATRIX_COLS; j++) {
            bool key_pressed = (current_matrix[i] >> j) & 1;

            if (key_pressed) {
                uprintf("{%s%u, %u%s}", red, ecsm_threshold[i][j].actuation , ecsm_threshold[i][j].release, reset);
            } else {
                uprintf("{%u, %u}", ecsm_threshold[i][j].actuation , ecsm_threshold[i][j].release);
            }

            if (j < MATRIX_COLS - 1) {
                uprintf(", ");
            }
        }
        if (i < MATRIX_ROWS - 1) {
            uprintf("},\\\n");
        } else {
            uprintf("}\\\n");
        }
    }
    uprintf("}\n\n");
}

void ecsm_print_matrix(matrix_row_t current_matrix[]) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        uprintf("[ADC readings], %u, ", i);
        for (int j = 0; j < MATRIX_COLS; j++) {
            bool key_pressed = (current_matrix[i] >> j) & 1;

            if (key_pressed) {
                uprintf("%s%4u%s," , red, ecsm_sw_value[i][j], reset);
            } else {
                uprintf("%4u," , ecsm_sw_value[i][j]);
            }
        }
        uprintf("\n");
    }
}

// Scan key values and update matrix state
bool ecsm_matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            ecsm_sw_value[row][col] = ecsm_readkey_raw(row, col);
            updated |= ecsm_update_key(&current_matrix[row], row, col, ecsm_sw_value[row][col]);
        }
    }
    ecsm_update_tuning = false;
    return updated;
}
