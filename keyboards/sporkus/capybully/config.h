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
// idle adc + offset = actuation/release threshold
// Larger offset = More key travel distance
#define ACTUATION_OFFSET 150
#define RELEASE_OFFSET 170

#define PRINT_ECSM_THRESHOLDS // enables printing key actutation threshold
#define PRINT_ECSM_READINGS   // enables printing matrix ADC readings
#define ECSM_UPDATE_CYCLES 20 // gather data for autotune every x matrix scans

// Starting config for EC matrix. Data format is ecsm_threshold_t[row][col] = {actuation, release}.
// paste console output of ecsm_print_thresholds() here. This should be updated after every assembly
#define ECSM_THRESHOLDS {\
   {{389, 339}, {522, 472}, {429, 379}, {481, 431}, {351, 301}, {388, 338}, {477, 427}, {461, 411}, {482, 432}, {410, 360}, {415, 365}, {417, 367}, {650, 600}},\
   {{392, 342}, {583, 533}, {468, 418}, {508, 458}, {431, 381}, {637, 587}, {619, 569}, {487, 437}, {620, 570}, {554, 504}, {647, 597}, {637, 587}, {650, 600}},\
   {{465, 415}, {590, 540}, {624, 574}, {483, 433}, {475, 425}, {454, 404}, {551, 501}, {521, 471}, {655, 605}, {541, 491}, {390, 340}, {438, 388}, {650, 600}},\
   {{556, 506}, {477, 427}, {464, 414}, {650, 600}, {504, 454}, {650, 600}, {441, 391}, {650, 600}, {520, 470}, {369, 319}, {383, 333}, {448, 398}, {650, 600}}\
}


// placeholder values before autotune applies, not important
#define DEFAULT_LOW_LEVEL 120
#define DEFAULT_HIGH_LEVEL 700
#define DEFAULT_ACTUATION_LEVEL 650
#define DEFAULT_RELEASE_LEVEL 600

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
