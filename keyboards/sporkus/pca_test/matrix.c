#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "print.h"
#include "matrix.h"
#include "i2c_master.h"
#include "pca9555.h"

// PCA95XX 7-bit address configuration:
// The last three bits can be changed. Only the last two bits are used here for up to
// 4 expanders (0 1 0 0 A2 A1 A0)
#define PCA_ADDR0 0x20 // A2=0 A1=0 A0=0
#define PCA_ADDR1 0x21 // A2=0 A1=0 A0=1
#define PCA_ADDR2 0x22 // A2=0 A1=1 A0=0
#define PCA_ADDR3 0x23 // A2=0 A1=1 A0=1
#define PCA_PIN_GET_BITMASK(pin) (uint8_t)1<<pin

typedef struct {
    uint8_t addr;
    uint8_t port;
    uint8_t value;
} pca_port_t;

typedef struct {
    uint8_t addr;
    uint8_t port;
    uint8_t pin;
} pca_pin_t;

#define PCA(pca_index, port, pin) {PCA_ADDR##pca_index, port, pin}
static pca_pin_t row_pins[] = ROW_PINS_PCA;
static pca_pin_t col_pins[] = COL_PINS_PCA;

// placeholder to be populated during matrix init
static pca_port_t row_ports[8];
static pca_port_t col_ports[8];
static uint8_t row_port_count;
static uint8_t col_port_count;

// QMK generated sections
#if (MATRIX_COLS<=8)
    typedef uint8_t matrix_row_t;
#elif (MATRIX_COLS<=16)
    typedef uint16_t matrix_row_t;
#elif (MATRIX_COLS<=32)
    typedef uint32_t matrix_row_t;
#endif


void validate_pca_config(void) {
    int matrix_rows = sizeof(row_pins) / sizeof(pca_pin_t);
    int matrix_cols = sizeof(col_pins) / sizeof(pca_pin_t);
    if (matrix_rows != MATRIX_ROWS || matrix_cols != MATRIX_COLS){
        uprintln("Invalid config: make sure that MATRIX_ROWS and MATRIX_COLS are correct.");
    }
    assert(matrix_rows == MATRIX_ROWS);
    assert(matrix_cols == MATRIX_COLS);

    bool pca_port_in_row_and_col = false;
    for (int r = 0; r < MATRIX_ROWS; r++) {
        for (int c = 0; c < MATRIX_COLS; c++) {
            pca_port_in_row_and_col |= (
                (row_pins[r].addr == col_pins[c].addr)
                && (row_pins[r].port == col_pins[c].port)
            );
        }
    }
    if(! pca_port_in_row_and_col){
        uprintln("Invalid config: Same PCA port is used for input and output.");
    };
}

void init_col_ports(void) {
    int port_count=1;
    col_ports[0] = (pca_port_t){.addr=col_pins[0].addr, .port=col_pins[0].port};
    pca9555_init(col_pins[0].addr);

    for (int i = 0; i < MATRIX_COLS; i++) {
        bool port_added = false;
        for (int j = 0; j < port_count; j++) {
            bool same_addr = col_pins[i].addr == col_ports[j].addr;
            bool same_port = col_pins[i].port == col_ports[j].port;
            port_added |= same_addr && same_port;
            if (port_added) {
                break;
            }
        }
        if (!port_added) {
            col_ports[port_count].addr = col_pins[i].addr;
            col_ports[port_count].port = col_pins[i].port;
            pca9555_init(col_pins[i].addr);
            port_count += 1;
        }
    }
    col_port_count = port_count;
}

void init_row_ports(void) {
    int port_count=1;
    row_ports[0].addr = row_pins[0].addr;
    row_ports[0].port = row_pins[0].port;
    pca9555_init(row_pins[0].addr);

    for (int i = 0; i < MATRIX_ROWS; i++) {
        bool port_added = false;
        for (int j = 0; j < port_count; j++) {
            bool same_addr = row_pins[i].addr == row_ports[j].addr;
            bool same_port = row_pins[i].port == row_ports[j].port;
            pca9555_init(row_pins[i].addr);
            port_added |= same_addr && same_port;
            if (port_added) {
                break;
            }
        }
        if (!port_added) {
            row_ports[port_count].addr = row_pins[i].addr;
            row_ports[port_count].port = row_pins[i].port;
            port_count += 1;
        }
    }
    row_port_count = port_count;
}

void print_config(void) {
    printf("%u col ports:\n", col_port_count);
    for (int i = 0; i < col_port_count; i++) {
        printf("addr: 0X%0X, port: %u, value: %u \n", col_ports[i].addr, col_ports[i].port, col_ports[i].value);
    }

    printf("%u row ports:\n", row_port_count);
    for (int i = 0; i < row_port_count; i++) {
        printf("addr: 0X%0X, port: %u\n", row_ports[i].addr, row_ports[i].port);
    }
}

void matrix_init_custom(void){
    /* validate_pca_config(); */
    uprintln("Hello");
    init_row_ports();
    init_col_ports();
    print_config();
}


int get_bit_state(uint32_t x, int bit) {
    uint32_t mask = (1 << bit);
    return (x & mask) != 0;
}

#if (DIODE_DIRECTION == COL2ROW)
static void select_row(uint8_t row) {
    pca_pin_t* row_pin = &row_pins[row];
    for (int i = 0; i < row_port_count; i++) {
        pca_port_t* curr_port = &row_ports[i];
        bool row_on_port = (row_pin->addr == curr_port->addr)
            && (row_pin->port == curr_port->port);

        if (row_on_port){
            // e.g: pin=2, bitmask: 00000100, output: 11111011
            uint8_t output = 0b11111111 & ~(1 << row_pin->pin);
            uprintf("setting output: 0x%X port:%u, value: 0X%X", curr_port->addr, curr_port->port, output);
            uprintln();
            pca9555_set_output(curr_port->addr, curr_port->port, output);
        } else {
            // set all other ports to high (logic low)
            pca9555_set_output(curr_port->addr, curr_port->port, ALL_HIGH);
        }
    }
}

static matrix_row_t read_cols(void) {
    matrix_row_t row_value = 0;
    for (int i = 0; i < col_port_count; i++) {
        pca_port_t* curr_port = &col_ports[i];
        pca9555_readPins(curr_port->addr, curr_port->port, &(curr_port->value));
        //map port readings to row value:
        for (int j = 0; j < MATRIX_COLS; j++) {
            pca_pin_t* curr_pin = &col_pins[j];
            if (curr_port->addr == curr_pin->addr && curr_port->port == curr_pin->port) {
                // Pin is logic high when the bit is low
                uint8_t pin_on = get_bit_state(curr_port->value, curr_pin->pin) ? 0 : 1;
                row_value |= (matrix_row_t)pin_on << j;
            }
        }
    }
    return row_value;
};

static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t curr_row) {
    /* uprintf("Reading row %u!!!!\n", curr_row); */
    matrix_row_t last_row_value = current_matrix[curr_row];
    current_matrix[curr_row] = 0;
    select_row(curr_row);
    current_matrix[curr_row] = read_cols();
    return last_row_value != current_matrix[curr_row];
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;

    for (uint8_t curr_row = 0; curr_row < MATRIX_ROWS; curr_row++) {
        matrix_has_changed |= read_cols_on_row(current_matrix, curr_row);
    }

    return matrix_has_changed;
}

#elif (DIODE_DIRECTION == ROW2COL)
static void select_col(uint8_t col) {
    pca_pin_t* curr_pin = col_pins[col];
    for (int i = 0; i < col_port_count; i++) {
        pca_port_t* curr_port = &col_ports[i];
        bool col_on_port = (curr_pin->addr == curr_port->adrr)
            && (curr_pin->port == curr_port->port);

        if (col_on_port){
            // e.g: pin=2, bitmask: 00000100, output: 11111011
            uint8_t output = 0b11111111 & ~(1 << pin);
            pca9555_set_output(curr_port->addr, curr_port->port, output);
        } else {
            // set all other ports to high (logic low)
            pca9555_set_output(curr_port->addr, curr_port->port, ALL_HIGH);
        }
    }
}

static uint32_t read_rows(void) {
    matrix_row_t col_value = 0;
    for (int i = 0; i < row_port_count; i++) {
        pca_port_t* curr_port = &row_ports[i];
        pca9555_readPins(curr_port->addr, curr_port->port, &(curr_port->value));
        //map port readings to row value:
        for (int j = 0; j < MATRIX_ROWS; j++) {
            pca_pin_t* curr_pin = &row_pins[j];
            if ((curr_port->addr == curr_pin->addr) && (curr_port->port == curr_pin->port)) {
                // Pin is logic high when the bit is low
                uint8_t pin_on = curr_port->value & PCA_PIN_GET_BITMASK(curr_pin->pin) ? 0 : 1;
                row_value |= (matrix_row_t)pin_on << j;
            }
        }
    }
    return col_value;
};

static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t curr_col) {
    uint32_t last_col_value = 0;
    uint32_t updated_col_value = 0;

    for (int i = 0; i < MATRIX_ROWS; i++) {
        int col_state = get_bit_state(current_matrix[i], curr_col);
        last_col_value |= (uint32_t)1 << i;
    }

    select_col(curr_col);
    updated_col_value = read_rows();

    for (int i = 0; i < MATRIX_ROWS; i++) {
        int col_state = get_bit_state(current_matrix[i], curr_col);
        if (col_state == 1) {
            current_matrix[i] |= 1 << curr_col; //set the bit to curr_col to 1
        } else {
            current_matrix[i] &= ~(1 << curr_col); //clear the bit at curr_col
        }
    }

    return last_col_value != updated_col_value;
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;

    for (uint8_t curr_row = 0; curr_row < MATRIX_ROWS; curr_row++) {
        matrix_has_changed |= read_cols_on_row(current_matrix, curr_row);
    }

    return matrix_has_changed;
}
#endif

