// SWIM.h — Sander-Wozniak Integrated Machine (floppy controller)
// Mac II uses SWIM for 400K/800K GCR and 1.4M MFM floppies

#ifndef SWIM_H
#define SWIM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t mode;       // IWM or SWIM mode
    uint8_t status;
    uint8_t handshake;
    bool motor_on;
    bool drive_sel;     // Internal/external drive
    int head;           // Current head (0 or 1)
    int track;          // Current track
    int sector;         // Current sector
} SWIMController;

void swim_init(SWIMController* swim);
void swim_reset(SWIMController* swim);
uint8_t swim_read(SWIMController* swim, uint32_t offset);
void swim_write(SWIMController* swim, uint32_t offset, uint8_t val);

bool swim_insert_disk(SWIMController* swim, const char* path, int drive);
void swim_eject_disk(SWIMController* swim, int drive);

#ifdef __cplusplus
}
#endif

#endif // SWIM_H
