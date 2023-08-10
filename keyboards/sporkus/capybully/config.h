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

#define MATRIX_ROWS 4
#define MATRIX_COLS 13

// --- Pin configuration ---
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

// --- EC matrix configuration ---

// Larger ratio = larger switch displacement
// baseline + setting * (max - baseline)
#define ACTUATION_RATIO 0.30
#define RELEASE_RATIO 0.27

// Starting config for EC matrix. Data format is ecsm_threshold_t[row][col] = {actuation, release}.
// paste console output of ecsm_print_thresholds() here
// this fucntion will run by default when ECSM_THRESHOLDS is not defined 
#define ECSM_THRESHOLDS {\
   {{421, 406}, {483, 469}, {463, 440}, {481, 459}, {453, 430}, {459, 436}, {625, 609}, {518, 498}, {496, 475}, {468, 446}, {493, 472}, {462, 447}, {650, 600}},\
   {{431, 411}, {572, 554}, {520, 500}, {506, 485}, {438, 414}, {569, 551}, {593, 576}, {551, 532}, {607, 590}, {562, 544}, {608, 593}, {608, 592}, {650, 600}},\
   {{499, 478}, {594, 577}, {593, 577}, {455, 432}, {581, 563}, {532, 513}, {589, 572}, {510, 490}, {594, 577}, {504, 484}, {390, 373}, {482, 464}, {650, 600}},\
   {{490, 468}, {568, 550}, {496, 475}, {650, 600}, {504, 483}, {650, 600}, {511, 490}, {650, 600}, {557, 538}, {396, 380}, {378, 361}, {517, 497}, {650, 600}}\
}

// force printing thresholds
#define PRINT_ECSM_THRESHOLDS 

// placeholder values before autotune applies, not important
#define DEFAULT_LOW_LEVEL 120
#define DEFAULT_HIGH_LEVEL 700
#define DEFAULT_ACTUATION_LEVEL 650
#define DEFAULT_RELEASE_LEVEL 600

// --- Misc ---
// #define DEBUG_MATRIX_SCAN_RATE