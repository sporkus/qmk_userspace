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

// EC specific configuration start ---------------------------------------------------

#define ACTUATION_DEPTH 60         // x % of travel
#define RELEASE_DEPTH 50           // x % of travel to release 
#define DEFAULT_IDLE 500           // default idle ADC before tuning completes
#define CALIBRATION_MIN_TRAVEL 10  // minimum travel as % of expected travel to count a key as bottomed
#define DEFAULT_BOTTOM_ADC 950     // assumed bottom ADC reading before bottoming calibration
#define TRAVEL_CURVE_GAMMA 1.5f    // power curve for actuation: >1 linearises EC's nonlinear capacitance response; 1.0 = linear
// #define ECSM_DEBUG         // enables periodic ADC/matrix dumps to console (noisy, use GUI instead)
#define EC_MATRIX            // allows ec code to be enabled with ifdef
//#define ECSM_TUNE_ON_BOOT  // enables tuning ec config every power cycle, disable to reduce write cycle to flash

// EC specific configuration end ---------------------------------------------------


// --- Pin configuration --------------------------------------

#define MATRIX_ROWS 4
#define MATRIX_COLS 13

#define EC_MATRIX_ROWS MATRIX_ROWS
#define EC_MATRIX_COLS MATRIX_COLS

#define EC_MATRIX_ROW_PINS \
    { C13, A2, A6, C14 }

/* offset the second multiplexer channel by 8 */
#define EC_MATRIX_COL_CHANNELS \
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

// --- Pin configuration end ----------------------------------

// --- Persistent Storage config ---
// ecsm_config_t layout (bytes):
//   bool configured (1) 
//   + bool bottoming_configured (1)
//   + int16_t actuation_offset (2) 
//   + int16_t release_offset (2)
//   + int16_t idle[MATRIX_ROWS][MATRIX_COLS] (4*13*2 = 104)
//   + uint16_t bottoming[EC_MATRIX_ROWS][EC_MATRIX_COLS] (4*13*2 = 104)
//   = 214 bytes
// NOTE: changing this struct layout requires EE_CLR on first flash
#define EECONFIG_KB_DATA_SIZE (6 + EC_MATRIX_ROWS * EC_MATRIX_COLS * 4)
