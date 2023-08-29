# Crin - custom 7u layout

* Keyboard Maintainer: [sporkus](https://github.com/sporkus)
* Hardware Supported: APM32F072
* Hardware Availability: [](https://github.com/sporkus/crin_keyboard)

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sporkus/crin -km default

Flashing example for this keyboard:

    qmk flash -kb sporkus/crin -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the DFU bootloader in 3 ways:

* **Physical DFU pads**: short the pads while powering up the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
* **Bootmagic**: Hold the top left key while powering up the keyboard
