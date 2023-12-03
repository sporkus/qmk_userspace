# Le Capybara

Electrocaptive sensing (Topre/Niz) PCB in the Le Chiffre laytout

* Keyboard Maintainer: [sporkus](https://github.com/sporkus)
* Hardware Supported: STM32F072
* Hardware design: (https://github.com/sporkus/le_capybara_keyboard)

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sporkus/le_capybara -km default

Flashing example for this keyboard:

    qmk flash -kb sporkus/le_capybara -km default

## Bootloader

Enter the DFU bootloader in 3 ways:
* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (the top left key) and plug in the keyboard
* **Physical DFU header**: Short the pads while powering up the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## EC Matrix functions

#### Tuning

The baseline for each key is different. Even if you use the same parts, they can change between assemblies. The very first time firmware is flashed, a quick auto tuning will be done (about half a minute). As long as you don't intentionally feather the keys, it should be immediately usable and not affect the tuning result.

To re-tune after assembly, you can either use the keycode `EC_CLR` to reset the configuration.
Actuation point adjustment

Use the keycodes `EC_AP_I`/`EC_AP_D` to increase/decrease the actuation point.
Advanced users

More options can be enabled in the config.h file.

- `ECSM_TUNE_ON_BOOT`: re-tune on every boot. It's not really necessary and increases write cycles to the flash memory.
- `ECSM_DEBUG`: prints EC config and readings in console.

