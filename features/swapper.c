#include "swapper.h"


void update_swapper(
    bool *active,
    uint16_t cmdish,
    uint16_t tabish,
    uint16_t trigger,
    uint16_t keycode,
    keyrecord_t *record
) {
    if (keycode == trigger) {
        if (record->event.pressed) {
            if (!*active) {
                *active = true;
                register_code(cmdish);
            }
            register_code(tabish);
            #ifdef CONSOLE_ENABLE
            uprintf("Swapper activated. holding mod...\n");
            #endif
        } else {
            unregister_code(tabish);
            // Don't unregister cmdish until some other key is hit or released.
        }
    } else if (*active) {
        if (record->event.pressed) {
            unregister_code(cmdish);
            #ifdef CONSOLE_ENABLE
            uprintf("Swapper deactivated. releasing mod...\n");
            #endif
            *active = false;
        }
    }
}
