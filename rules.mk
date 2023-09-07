# Enable common features
CONSOLE_ENABLE = yes
CAPS_WORD_ENABLE = yes
COMBO_ENABLE = yes
EXTRAKEY_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
DYNAMIC_MACRO_ENABLE = yes
AUTO_SHIFT_ENABLE = yes
LEADER_ENABLE = yes
OS_DETECTION_ENABLE = yes
DEFERRED_EXEC_ENABLE = yes

INTROSPECTION_KEYMAP_C += sporkus.c
SRC += features/swapper.c
SRC += features/global_quick_tap.c

