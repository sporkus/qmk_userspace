// Copyright @sporkus
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include QMK_KEYBOARD_H
#include "os_detection.h"
#include "layout.h"
// #define GLOBAL_QUICK_TAP_ENABLE
// #include "features/global_quick_tap.h"
// #define SWAPPER_ENABLE
// #include "features/swapper.h"

#define IS_HOMEROW(r) (r->event.key.row == 1)
