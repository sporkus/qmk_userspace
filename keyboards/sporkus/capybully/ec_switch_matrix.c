/* Copyright 2023 Cipulot
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
#include "atomic_util.h"
#include "print.h"
#include "wait.h"

/* Pin and port array */
const uint32_t row_pins[]     = MATRIX_ROW_PINS;
const uint8_t  col_channels[] = MATRIX_COL_CHANNELS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS;
const uint32_t mux_en_pins[] =  MUX_EN_PINS;

static ecsm_config_t config;
static uint16_t      ecsm_sw_value[MATRIX_ROWS][MATRIX_COLS];

static adc_mux adcMux;

static inline void discharge_capacitor(void) {
    writePinLow(DISCHARGE_PIN);
    wait_us(DISCHARGE_TIME);
}
static inline void charge_capacitor(uint8_t row) {
    // Blocks discharge route
    writePinHigh(DISCHARGE_PIN);
    // Send signal to row
    writePinHigh(row_pins[row]);
}

static inline void disable_mux_all(void) {
    for (int i = 0; i < 2; i++) {
        writePinHigh(mux_en_pins[i]);
    }
}

static inline void enable_mux(uint8_t index) {
    writePinLow(mux_en_pins[index]);
}

static inline void select_col(uint8_t col) {
    disable_mux_all();

    uint8_t ch = col_channels[col];
    writePin(mux_sel_pins[0], ch & 1);
    writePin(mux_sel_pins[1], ch & 2);
    writePin(mux_sel_pins[2], ch & 4);

    uint8_t mux_index = ((ch & (1<<3)) != 0);
    enable_mux(mux_index);
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
int ecsm_init(ecsm_config_t const* const ecsm_config) {
    // Initialize config
    config = *ecsm_config;

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

int ecsm_update(ecsm_config_t const* const ecsm_config) {
    // Save config
    config = *ecsm_config;
    return 0;
}

// Read the capacitive sensor value
uint16_t ecsm_readkey_raw(uint8_t row, uint8_t col) {
    uint16_t sw_value = 0;

    select_col(col);
    // Set strobe pins to low state
    writePinLow(row_pins[row]);
    ATOMIC_BLOCK_FORCEON {
        // Set the row/strobe pin high
        charge_capacitor(row);
        sw_value = adc_read(adcMux);
    }
    // reset strobe pin
    writePinLow(row_pins[row]);
    // Discharge peak hold capacitor
    discharge_capacitor();

    return sw_value;
}

// Update press/release state of key
bool ecsm_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value) {
    bool current_state = (*current_row >> col) & 1;

    // Press to release
    if (current_state && sw_value < config.ecsm_actuation_threshold) {
        *current_row &= ~(1 << col);
        return true;
    }

    // Release to press
    if ((!current_state) && sw_value > config.ecsm_release_threshold) {
        *current_row |= (1 << col);
        return true;
    }

    return false;
}

// Scan key values and update matrix state
bool ecsm_matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = false;

    for (int col = 0; col < sizeof(col_channels); col++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            ecsm_sw_value[row][col] = ecsm_readkey_raw(row, col);
            updated |= ecsm_update_key(&current_matrix[row], row, col, ecsm_sw_value[row][col]);
        }
    }

    return updated;
}

// Debug print key values
void ecsm_print_matrix(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            uprintf("%4d", ecsm_sw_value[row][col]);
            if (col < (MATRIX_COLS - 1)) {
                print(",");
            }
        }
        print("\n");
    }
    print("\n");
}
