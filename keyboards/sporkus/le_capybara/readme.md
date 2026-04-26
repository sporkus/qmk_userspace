# Le Capybara

Electrocapacitive sensing (Topre/Niz) PCB in the Le Chiffre layout.

* Keyboard Maintainer: [sporkus](https://github.com/sporkus)
* Hardware Supported: STM32F072
* Hardware design: https://github.com/sporkus/le_capybara_keyboard

```
qmk compile -kb sporkus/le_capybara -km sporkus
qmk flash   -kb sporkus/le_capybara -km sporkus
```

## Bootloader

Enter the DFU bootloader in 3 ways:
* **Bootmagic reset**: Hold the top-left key while plugging in
* **Physical DFU header**: Short the pads while powering up
* **Keycode**: Press `QK_BOOT` if mapped (combo: Q+W+O+P)

## Custom matrix driver (EC)

This keyboard uses electrocapacitive sensing instead of standard switch contacts. `rules.mk` sets:

```makefile
CUSTOM_MATRIX = lite
SRC += matrix.c analog.c ec_switch_matrix.c
```

`CUSTOM_MATRIX = lite` replaces QMK's GPIO matrix scan with custom ADC reads while keeping QMK's debounce logic. The three source files handle: analog discharge sequencing → ADC sampling → threshold comparison → key state.

## EC tuning

Each key has a different capacitive baseline depending on assembly. On first flash, auto-tuning runs (~30 seconds) to measure idle values per key. Hold keys still during this.

After running bottoming calibration (`EC_CAL`), actuation depth is expressed as a percentage of per-key travel — e.g. 25% fires the key a quarter of the way down.

| Keycode                   | Action                                              |
|---------------------------|-----------------------------------------------------|
| `EC_DEEPER` / `EC_AP_I`   | Require deeper press to actuate (less sensitive)    |
| `EC_SHALLOWER` / `EC_AP_D`| Require shallower press to actuate (more sensitive) |
| `EC_CAL`                  | Toggle bottoming calibration (start / save)         |
| `EC_CLR`                  | Reset stored EC config, re-tune on next boot        |
| `EE_CLR`                  | Full EEPROM reset                                   |

Configured in `config.h`:

```c
#define ACTUATION_DEPTH 50     // 50% of key travel (after bottoming cal), or raw ADC units before
#define RELEASE_DEPTH   40     // 40% — shallower than actuation, must lift past here to de-actuate
#define ECSM_TUNE_ON_BOOT      // re-tune every boot (more flash writes)
#define ECSM_DEBUG             // print EC readings to console
#define EC_MATRIX              // guard for #ifdef EC_MATRIX in shared code
```

## RGB

11 LEDs (9 underglow + 2 front indicators). If front LEDs are not installed and bypassed with the solder jumper, define `FRONT_LEDS_BYPASS` in `config.h` to reduce LED count to 9.

Front LEDs serve as indicators when enabled:
- `RGB_MODS_INDICATOR_ENABLE` — lights on mod keys while held
- `RGB_LAYER_INDICATOR_ENABLE` — shows active layer

