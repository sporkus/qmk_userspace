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

// RGB ----------------------------------------------------

#define ENCODER_MAP_ENABLE

#define WS2812_DI_PIN A15
#define RGB_MATRIX_LED_COUNT 11
#define DRIVER_LED_TOTAL RGB_MATRIX_LED_COUNT

// #define FRONT_LEDS_BYPASS  //  if front leds are not installed and are bypassed with solder jumper
#ifdef FRONT_LEDS_BYPASS
    #undef RGB_MATRIX_LED_COUNT
    #define RGB_MATRIX_LED_COUNT 9
#endif

// The following two options are for disabling the front leds if they're installed.
// Comment these out to disable them. 
#define RGB_MODS_INDICATOR_ENABLE
#define RGB_LAYER_INDICATOR_ENABLE

#define RGB_FRONT_BRIGHTNESS_SCALE 3  // Front indicator brightness scaling (0-10)
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES
#define RGB_TRIGGER_ON_KEYDOWN
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 255
#define RGB_MATRIX_HUE_STEP 8
#define RGB_MATRIX_SAT_STEP 8
#define RGB_MATRIX_VAL_STEP 16
#define RGB_MATRIX_SPD_STEP 16
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CYCLE_ALL
#define RGB_MATRIX_DEFAULT_HUE 0
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL 100
#define RGB_MATRIX_DEFAULT_SPD 32

#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_HUE_BREATHING
#define ENABLE_RGB_MATRIX_HUE_PENDULUM

// RGB end ------------------------------------------------


// --- Pin configuration --------------------------------------

#define MATRIX_ROWS 5
#define MATRIX_COLS 10
#define EC_MATRIX_ROWS 4
#define EC_MATRIX_COLS MATRIX_COLS

#define EC_MATRIX_ROW_PINS \
    { A3, A5, A7, B1 }

/* offset the second multiplexer channel by 8 */
#define EC_MATRIX_COL_CHANNELS \
    { \
       0, 1, 2, 3, 4, \
       10, 12, 14, 15, 13 \
    }

#define MUX_SEL_PINS \
    { B15, B14, B13 }

#define MUX_EN_PINS \
    { A8, C14 }

#define DISCHARGE_PIN A1 // opamp v+/mux output
#define ANALOG_PORT A2   // opamp v_out/adc

// extra list of direct pins to read (non-EC)
#define EXTRA_SWITCHES 1                      // number of extra switches
#define EXTRA_SWITCH_ROW (MATRIX_ROWS - 1)    // the last row
#define EXTRA_SWITCH_PINS {B3}

// --- Pin configuration end ----------------------------------

// --- Persistent Storage config ---
// ecsm_config_t layout (bytes):
//   bool configured (1) 
//   + bool bottoming_configured (1)
//   + int16_t actuation_offset (2) 
//   + int16_t release_offset (2)
//   + int16_t idle[EC_MATRIX_ROWS][EC_MATRIX_COLS] (4*10*2 = 80)
//   + uint16_t bottoming[EC_MATRIX_ROWS][EC_MATRIX_COLS] (4*10*2 = 80)
//   = 166 bytes
// NOTE: changing this requires EE_CLR on first flash
#define EECONFIG_KB_DATA_SIZE (6 + EC_MATRIX_ROWS * EC_MATRIX_COLS * 4)
