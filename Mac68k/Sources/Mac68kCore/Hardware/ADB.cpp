// ADB.cpp — Apple Desktop Bus controller for Mac II

#include "ADB.h"
#include <cstring>

// ADB command types
enum ADBCommand {
    ADB_RESET    = 0x00,
    ADB_FLUSH    = 0x01,
    ADB_LISTEN   = 0x08,
    ADB_TALK     = 0x0C
};

// ADB device addresses
enum ADBAddress {
    ADB_KEYBOARD = 2,
    ADB_MOUSE    = 3
};

void adb_init(ADBController* adb) {
    memset(adb, 0, sizeof(ADBController));
}

void adb_reset(ADBController* adb) {
    adb_init(adb);
}

void adb_command(ADBController* adb, uint8_t cmd) {
    adb->address = (cmd >> 4) & 0x0F;
    adb->command = (cmd >> 2) & 0x03;
    adb->reg = cmd & 0x03;
}

uint8_t adb_response(ADBController* adb, uint8_t* data, int* len) {
    *len = 0;

    if (adb->command != (ADB_TALK >> 2)) return 0;

    if (adb->address == ADB_MOUSE && adb->reg == 0) {
        // Mouse data: 2 bytes — button + delta
        int dx = adb->mouse_dx;
        int dy = adb->mouse_dy;
        if (dx == 0 && dy == 0 && !adb->mouse_button) return 0;

        // Clamp to 7-bit signed range
        if (dx > 63) dx = 63;
        if (dx < -64) dx = -64;
        if (dy > 63) dy = 63;
        if (dy < -64) dy = -64;

        data[0] = (adb->mouse_button ? 0x00 : 0x80) | (dy & 0x7F);
        data[1] = 0x80 | (dx & 0x7F); // Second button always up
        *len = 2;

        adb->mouse_dx = 0;
        adb->mouse_dy = 0;
        return 1;
    }

    if (adb->address == ADB_KEYBOARD && adb->reg == 0) {
        // Keyboard data: 2 bytes — up to 2 key events
        if (adb->key_count == 0) return 0;

        data[0] = adb->key_buffer[0];
        data[1] = (adb->key_count > 1) ? adb->key_buffer[1] : 0xFF;
        *len = 2;

        // Shift buffer
        int consumed = (adb->key_count > 1) ? 2 : 1;
        adb->key_count -= consumed;
        if (adb->key_count > 0) {
            memmove(adb->key_buffer, adb->key_buffer + consumed,
                    adb->key_count);
        }
        return 1;
    }

    return 0;
}

void adb_mouse_move(ADBController* adb, int dx, int dy) {
    adb->mouse_dx += dx;
    adb->mouse_dy += dy;
}

void adb_mouse_button(ADBController* adb, bool down) {
    adb->mouse_button = down;
}

void adb_key_event(ADBController* adb, uint8_t keycode, bool down) {
    if (adb->key_count >= 16) return;
    // ADB key format: bit 7 = 0 for down, 1 for up
    adb->key_buffer[adb->key_count++] = down ? keycode : (keycode | 0x80);
}
