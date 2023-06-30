# Notes


## Creating keymap for new keyboard

### `[keyboard].json`
This is the file that we will be running `qmk compile/flash` on. Example:

```
qmk compile ./keymaps/[keyboard].json
```

Keys:
 -  `keyboard`: the original keyboard in `qmk_firmware/keyboards`
 -  `keymap`: `[your_username` in `qmk_firmware/users`
 -  `layout`: this is where we reference a layout wrapper defined in `[keyboard].h`
 -  `layers`: this is where we reference the layers defined in `[keyboard].h`

### `[keyboard]_layout.h`
This is how we shape the base layout macros into the layout for this particular keyboard

### `config.h`
Use conditional include for the keyboard specific layout wrapper. Example:

```
#ifdef KEYBOARD_[parent]_[keyboard]
# include "[keyboard].h"
#endif

```
