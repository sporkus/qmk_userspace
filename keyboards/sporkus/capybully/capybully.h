#pragma once

#include "quantum.h"
#include "ec_switch_matrix.h"

enum ec_keycodes {
    EC_UPDATE = QK_KB_0,  // saves actuation point to persistent storage
    EC_AP_INC,            // Increases actuation point (more travel)
    EC_AP_DEC,            // Decreases actuation point
};
