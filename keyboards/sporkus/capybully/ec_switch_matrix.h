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

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "matrix.h"

typedef struct {
    uint16_t actuation; // threshold for key release
    uint16_t release;   // threshold for key press
} ecsm_threshold_t;

typedef struct {
    uint16_t adc_readings[30];  // hold the last 30 readings
    uint16_t adc_max;
    uint16_t adc_median;
} ecsm_tune_data_t;


int      ecsm_init(void);
bool     ecsm_matrix_scan(matrix_row_t current_matrix[]);
uint16_t ecsm_readkey_raw(uint8_t row, uint8_t col);
bool     ecsm_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value);
void     ecsm_print_matrix(void);
