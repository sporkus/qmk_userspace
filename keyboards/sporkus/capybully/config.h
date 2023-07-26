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


#define DISCHARGE_TIME 10
#define DEFAULT_ACTUATION_LEVEL 550
#define DEFAULT_RELEASE_LEVEL 500
