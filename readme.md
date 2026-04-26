# QMK Userspace — sporkus

Personal QMK userspace ([sporkus](https://github.com/sporkus)). Manages keymaps for multiple keyboards from a single repo, overlaid on a local QMK installation.

See [QMK external userspace docs](https://docs.qmk.fm/newbs_external_userspace) for setup.

---

## Building and flashing

```
qmk compile -kb sporkus/le_capybara -km sporkus
qmk flash   -kb sporkus/le_capybara -km sporkus
```

Keyboard paths: `keyboards/[creator]/[kb_name]`  
Keymap paths: `keyboards/[creator]/[kb_name]/keymaps/[keymap_name]`

---

## File structure and import flow

Important files for updating keymap:

```
layout.h              ← all layer definitions, keycodes, HRM macros
combos.def            ← combo definitions
key_overrides.def     ← key overrides definitions
sporkus.c             ← additional runtime logic: process_record_user, tap dance, etc.
keyboards/sporkus/[kb]/keymaps/sporkus/keymap.c  ← per-keyboard layout wiring
```

### layout.h (project root)

Defines everything shared across all keyboards:

- **Layer macros** (`_BASE`, `_NUM`, `_NAV`, `_FN`): flat 34-key keymaps as comma-separated keycodes
- **Home row mod macros** (`HRML`, `HRMR`, `BRML`, `BRMR`): wrap a half-row to apply mod-tap
- **Row extraction helpers** (`LEFT_ROW1`, `RIGHT_ROW2`, etc.): slice a flat layer macro into individual rows


Each keyboard's `keymap.c` includes `layout.h` and uses these helpers to feed the right keys into the keyboard-specific `LAYOUT()` macro. For example in `keyboards/sporkus/capybully/keymaps/sporkus/keymap.c`:

```c
#include "layout.h"

[0] = LAYOUT_w(
    LEFT_ROW1(_BASE),  KC_MUTE,  RIGHT_ROW1(_BASE),   // encoder key inserted between halves
    HRML(LEFT_ROW2(_BASE)),       HRMR(RIGHT_ROW2(_BASE)),
    ...
)
```

### combos.h / combos.def

`combos.def` is the only file you edit to add or change combos:

```c
//name,       output,     term,  keys
CMB(ESC,      KC_ESC,     25,    HM_S, HM_D)
CMB(ENTER,    KC_ENTER,   25,    HM_J, HM_K)
```

`combos.h` is a code generator — it includes `combos.def` four times, each time with a different definition of the `CMB()` macro, to produce:

1. The `enum combos { C_ESC, C_ENTER, ... }` values
2. The `PROGMEM` key arrays: `const uint16_t esc_combo[] = { HM_S, HM_D, COMBO_END }`
3. The `combo_t key_combos[]` array passed to QMK
4. The `get_combo_term()` switch statement for per-combo timing

You never need to touch `combos.h`. Just add rows to `combos.def`.

### key_overrides.h / key_overrides.def

Same pattern as combos. `key_overrides.def` is the only file to edit:

```c
//KEY_OVERRIDE(name,          mod_mask,        keycode,   modded_keycode)
KEY_OVERRIDE(comma_scln,      MOD_MASK_SHIFT,  KC_COMMA,  KC_SCLN)
```

`key_overrides.h` includes it twice to generate the `const key_override_t` structs and the `key_overrides[]` array that QMK reads.

---

## User rules.mk

The project-root `rules.mk` sets feature flags and adds shared source files that apply to every keyboard:

```makefile
COMBO_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
INTROSPECTION_KEYMAP_C += sporkus.c
SRC += features/swapper.c
```


## Keyboard rules.mk
`rules.mk` in each keyboard folder provides additional source code or overrides upstream ones.
Example: EC keyboards (le_capybara, capybully) require a custom matrix driver because they use electrocapacitive (Topre/Niz) sensing instead of standard switch matrices. 

```makefile
# keyboards/sporkus/le_capybara/rules.mk
CUSTOM_MATRIX = lite
SRC += matrix.c analog.c ec_switch_matrix.c
```

`CUSTOM_MATRIX = lite` tells QMK to use your `matrix.c` for scanning while keeping its debounce logic. The three source files implement analog ADC reads → EC switch detection → key state, replacing the default GPIO-based matrix entirely.

---

## Features

- Shared 34-key base layout across all keyboards via `layout.h`
- Home row mods with per-keyboard wrapping
- Combos defined in `combos.def` (X-macro generated)
- Key overrides defined in `key_overrides.def` (X-macro generated)
- Custom EC matrix driver for Topre/Niz keyboards
- Swapper, global quick tap, num word features in `features/`
- GitHub Actions build with QMK and Vial support

---

## Credits

- filterpaper
- sadekbaroudi
- geutreur
- replicaJunction
