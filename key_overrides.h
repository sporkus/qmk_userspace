// Copyright 2023 sporkus
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#define OVERRIDE_DEF "key_overrides.def"

#define KEY_OVERRIDE(name, modmask, keycode, modded_keycode) \
const key_override_t name##_key_override = ko_make_basic(modmask, keycode, modded_keycode);
#include OVERRIDE_DEF
#undef KEY_OVERRIDE

#define KEY_OVERRIDE(name, ...) &name##_key_override,
const key_override_t **key_overrides = (const key_override_t *[]){
    #include OVERRIDE_DEF
    NULL
};
#undef KEY_OVERRIDE

#undef OVERRIDE_DEF
