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

// #define DEBUG_MATRIX_SCAN_RATE

// Larger ratio = larger switch displacement
// baseline + setting * (max - baseline)
#define ACTUATION_RATIO 0.30
#define RELEASE_RATIO 0.27

// inital values before autotune applies, not critical
#define DEFAULT_LOW_LEVEL 120
#define DEFAULT_HIGH_LEVEL 700
#define DEFAULT_ACTUATION_LEVEL 650
#define DEFAULT_RELEASE_LEVEL 600

// paste console output of ecsm_print_thresholds() here
// this fuciont will run if ECSM_THRESHOLDS is not defined 
#define ECSM_THRESHOLDS {\
   {{474, 455}, {483, 473}, {461, 439}, {489, 467}, {446, 423}, {468, 445}, {627, 611}, {530, 511}, {501, 480}, {474, 451}, {496, 475}, {462, 447}, {650, 600}},\
   {{650, 600}, {573, 555}, {518, 498}, {503, 482}, {438, 415}, {577, 559}, {593, 576}, {553, 535}, {608, 591}, {559, 540}, {618, 602}, {608, 592}, {650, 600}},\
   {{504, 483}, {594, 577}, {615, 598}, {464, 442}, {560, 542}, {536, 516}, {593, 576}, {502, 481}, {594, 577}, {510, 489}, {390, 373}, {499, 480}, {650, 600}},\
   {{490, 468}, {568, 550}, {496, 475}, {650, 600}, {504, 483}, {650, 600}, {513, 492}, {650, 600}, {557, 538}, {396, 380}, {378, 361}, {527, 507}, {650, 600}}\
}

// force printing thresholds
#define PRINT_ECSM_THRESHOLDS 
