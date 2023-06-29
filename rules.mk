# Disable unused features
MAGIC_ENABLE = no
UNICODE_ENABLE = no
SPACE_CADET_ENABLE = no

# Enable common features
CONSOLE_ENABLE = no
CAPS_WORD_ENABLE = yes
COMBO_ENABLE = yes
EXTRAKEY_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
DYNAMIC_MACRO_ENABLE = yes
AUTO_SHIFT_ENABLE = yes
LEADER_ENABLE = yes

INTROSPECTION_KEYMAP_C += sporkus.c
SRC += features/swapper.c
SRC += features/global_quick_tap.c

