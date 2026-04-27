#pragma once

#include "quantum.h"
#include "ec_switch_matrix.h"

enum ec_keycodes {
    EC_AP_I = QK_KB_0,  // Require deeper press to actuate (less sensitive)
    EC_AP_D,            // Require shallower press to actuate (more sensitive)
    EC_CLR,             // Reset EC config
    EC_CAL,             // Toggle per-key bottoming calibration mode
    EC_TUI              // Toggle structured ADC streaming for the TUI tool
};

// Descriptive aliases — prefer these in new keymaps
#define EC_DEEPER    EC_AP_I  // Increase actuation depth (less sensitive)
#define EC_SHALLOWER EC_AP_D  // Decrease actuation depth (more sensitive)
