# Capybully 

Electrocaptive sensing (Topre/Niz) PCB for the [Bully](https://cbkbd.bigcartel.com/product/bully) keyboard

* Keyboard Maintainer: [sporkus](https://github.com/sporkus)
* Hardware Supported: STM32F072
* Hardware Availability: [](https://github.com/sporkus/le_chiffre_keyboard_stm32)

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sporkus/capybully -km default

Flashing example for this keyboard:

    qmk flash -kb sporkus/capybully -km default

## Bootloader

Enter the DFU bootloader in 3 ways:
* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (the top left key) and plug in the keyboard
* **Physical DFU header**: Short the pads while powering up the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
