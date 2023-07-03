#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

// Assumptions: all IO will be done on PCA95XX ports
// Each IO port is used for either input or output, not mixed.
// Up to 4 PCA95XX (64 pins) can be used for the matrix.
#define MATRIX_ROWS 2
#define MATRIX_COLS 2
#define DIODE_DIRECTION COL2ROW

//PCA0 = 0x20, PCA1 = 0x21, PCA2 = 0x22, PCA3 = 0x23
//PCA(expander index, port, pin)
#define ROW_PINS_PCA { \
    PCA(0, 0, 0), \
    PCA(0, 0, 1) \
}

#define COL_PINS_PCA { \
    PCA(0, 1, 0), \
    PCA(0, 1, 1) \
}

#define I2C1_DRIVER I2CD1
#define I2C1_SDA_PIN GP6
#define I2C1_SCL_PIN GP7
