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
    uint16_t low;     // ADC reading at idle
    uint16_t high;    // max ADC reading
} ecsm_tune_data_t;

extern bool ecsm_update_tuning;


int      ecsm_init(void);
bool     ecsm_matrix_scan(matrix_row_t current_matrix[]);
uint16_t ecsm_readkey_raw(uint8_t row, uint8_t col);
bool     ecsm_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value);
void     ecsm_update_tune_data(uint16_t new_data, uint8_t, uint8_t col);
void     ecsm_update_threshold(void);
void     ecsm_print_tuning(void);
void     ecsm_print_matrix(matrix_row_t current_matrix[]);
void     ecsm_print_thresholds(matrix_row_t current_matrix[]);
