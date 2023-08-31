#pragma once

#include "quantum.h"
#include "ec_switch_matrix.h"

enum ec_keycodes {
    EC_AP_I = USER00,  // Increases actuation point (more travel)
    EC_AP_D,            // Decreases actuation point
};
