// Copyright 2023 sporkus
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "layout.h"

#define COMBOS_DEF "combos.def"
#define CMB(name, action, ...) C_##name,
enum combos {
    #include COMBOS_DEF
    COMBO_LENGTH
};
#undef CMB

#define CMB(name, output, term, ...) const uint16_t PROGMEM name##_combo[] = {__VA_ARGS__, COMBO_END};
#include COMBOS_DEF
#undef CMB

#define CMB(name, output, term, ...) COMBO(name##_combo, output),
uint16_t COMBO_LEN = COMBO_LENGTH;
combo_t key_combos[COMBO_LENGTH] = {
    #include COMBOS_DEF
};
#undef CMB

#define CMB(name, output, term, ...) case C_##name:\
        return term;
uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    switch (index) {
        #include COMBOS_DEF
    }
    return COMBO_TERM;
}
#undef CMB

#undef COMBOS_DEF
