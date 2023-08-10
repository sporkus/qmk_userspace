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
#include "matrix.h"
#include "print.h"

/* matrix state(1:on, 0:off) */
extern matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
extern matrix_row_t matrix[MATRIX_ROWS];     // debounced values

void matrix_init_custom(void) {
    ecsm_init();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool updated = ecsm_matrix_scan(current_matrix);

// RAW matrix values on console
    static int cnt = 0;

    if (cnt % 10 == 0) {
        ecsm_update_tuning = true;
    }

    if (cnt++ == 5000) {
        cnt = 0;
        ecsm_update_threshold();
#ifdef CONSOLE_ENABLE
    #if !defined(ECSM_THRESHOLDS) || defined(PRINT_ECSM_THRESHOLDS)
        ecsm_print_thresholds();
    #endif
        ecsm_print_matrix();
#endif
    }

    return updated;
}
