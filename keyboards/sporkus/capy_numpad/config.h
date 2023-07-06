// Copyright 2023 spork (@spork)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* Double tap reset button to enter bootloader */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U


#define MATRIX_COLS 4
#define MATRIX_ROWS 4

#define CAP_SAMPLE_PIN GP1
#define CAP_CHARGE_PIN GP1
#define A_MUX0 GP1
#define A_MUX1 GP1