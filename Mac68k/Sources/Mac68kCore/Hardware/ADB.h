// ADB.h — Apple Desktop Bus controller
//
// Mac II uses the ADB via VIA1 shift register
// ADB devices: keyboard (address 2), mouse (address 3)

#ifndef ADB_H
#define ADB_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // Mouse state
    int16_t mouse_dx;
    int16_t mouse_dy;
    bool mouse_button;

    // Keyboard state
    uint8_t key_buffer[16];
    int key_count;

    // ADB state machine
    uint8_t command;
    uint8_t address;
    uint8_t reg;
    uint8_t state;
    uint8_t data[8];
    int data_len;
} ADBController;

void adb_init(ADBController* adb);
void adb_reset(ADBController* adb);

// Host → ADB command processing
void adb_command(ADBController* adb, uint8_t cmd);
uint8_t adb_response(ADBController* adb, uint8_t* data, int* len);

// Input from user
void adb_mouse_move(ADBController* adb, int dx, int dy);
void adb_mouse_button(ADBController* adb, bool down);
void adb_key_event(ADBController* adb, uint8_t keycode, bool down);

#ifdef __cplusplus
}
#endif

#endif // ADB_H
