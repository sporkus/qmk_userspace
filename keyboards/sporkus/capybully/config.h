/* Copyright 2023 sporkus
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

#pragma once

// --- EC matrix user configuration ---
// Actuation/release threshold = idle + offset
// Larger offset = More key travel distance, less sensitive
// Sensitivity can be adjusted using keycode: EC_AP_I/EC_AP_D
#define ACTUATION_OFFSET 150
#define RELEASE_OFFSET 170
#define DEFAULT_IDLE 500      // default value before tuning is completed
//#define ECSM_DEBUG         // enables printing ec config and ADC readings
#define EC_MATRIX            // allows ec code to be enabled with ifdef

// enables tuning ec config every power cycle, disable to reduce write cycle to flash
//#define ECSM_TUNE_ON_BOOT

// --- Misc ---
// #define DEBUG_MATRIX_SCAN_RATE

// --- Pin configuration ---
#define MATRIX_ROWS 4
#define MATRIX_COLS 13

#define MATRIX_ROW_PINS \
    { C13, A2, A6, C14 }

/* offset the second multiplexer channel by 8 */
#define MATRIX_COL_CHANNELS \
    { \
       3, 0, 1, 2, 4, 6, \
       11, 8, 9, 10, 12, 14, 13 \
    }

#define MUX_SEL_PINS \
    { B11, B10, B2 }

#define MUX_EN_PINS \
    { B1, B0 }

#define DISCHARGE_PIN A4
#define ANALOG_PORT A3

// --- Persistent Storage config ---
// Data size is in bytes. uint16_t = 2 bytes
// data block size needs to be uint16_t array length * 2
// Two addition words for actuation offsets and one byte for configuration check
#define EECONFIG_KB_DATA_SIZE ((MATRIX_ROWS * MATRIX_COLS + 2) * 2 + 1)
