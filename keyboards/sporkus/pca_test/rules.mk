# This file intentionally left blank
CONSOLE_ENABLE=yes

CUSTOM_MATRIX = lite
VPATH += drivers/gpio
SRC += matrix.c pca9555.c
QUANTUM_LIB_SRC += i2c_master.c
OPT_DEFS += -DHAL_USE_I2C=TRUE
