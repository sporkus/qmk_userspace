#include "le_capybara.h"
#include "raw_hid.h"

extern ecsm_config_t ecsm_config;

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch(keycode) {
            case EC_AP_I:
                ecsm_ap_inc();
                return false;
            case EC_AP_D:
                ecsm_ap_dec();
                return false;
            case EC_CLR:
                ecsm_eeprom_clear();
                return false;
            case EC_CAL:
                ecsm_bottoming_cal_toggle();
                return false;
            case EC_TUI:
                ecsm_tui_toggle();
                return false;
        }
    }

    return process_record_user(keycode, record);
};


// EC tool HID commands - must match ec_calibration.html
#define EC_HID_PREFIX     0xEC
#define EC_HID_KEEPALIVE  0x00
#define EC_HID_CAL_TOGGLE 0x02
#define EC_HID_AP_INC     0x03
#define EC_HID_AP_DEC     0x04
#define EC_HID_JAB_TOGGLE 0x05
#define EC_HID_SET_BOTTOM 0x06
#define EC_HID_SET_IDLE   0x07

// Only stamps a timer - no raw_hid_send allowed inside the receive callback (via.c constraint).
// ec_hid_task() in ec_switch_matrix.c reads the timer from the scan loop and does the toggle there.
static void ec_hid_cmd(uint8_t *data, uint8_t length) {
    if (length < 2 || data[0] != EC_HID_PREFIX) return;
    switch (data[1]) {
        case EC_HID_KEEPALIVE:  ec_hid_keepalive();             break;
        case EC_HID_CAL_TOGGLE: ecsm_bottoming_cal_toggle();    break;
        case EC_HID_AP_INC:     ecsm_ap_inc();                  break;
        case EC_HID_AP_DEC:     ecsm_ap_dec();                  break;
        case EC_HID_JAB_TOGGLE: ecsm_jab_toggle(data[2], data[3]); break;
        case EC_HID_SET_BOTTOM: ecsm_set_bottom(data[2], data[3], ((uint16_t)data[4] << 8) | data[5]); break;
        case EC_HID_SET_IDLE:   ecsm_set_idle  (data[2], data[3], ((uint16_t)data[4] << 8) | data[5]); break;
    }
    // Zero prefix so via.c echoes all-zeros back; onReport skips zero-only packets.
    data[0] = 0x00;
    data[1] = 0x00;
}

#ifdef VIAL_ENABLE
void raw_hid_receive_kb(uint8_t *data, uint8_t length) { ec_hid_cmd(data, length); }
#else
void raw_hid_receive(uint8_t *data, uint8_t length) { ec_hid_cmd(data, length); }
#endif

void keyboard_post_init_kb(void) {
    #ifdef ECSM_TUNE_ON_BOOT
        ecsm_config.configured = 0;
        eeconfig_update_kb_datablock(&ecsm_config, 0, sizeof(ecsm_config));
    #endif
    keyboard_post_init_user();
}
