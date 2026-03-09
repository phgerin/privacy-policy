// SWIM.cpp — Floppy controller stub

#include "SWIM.h"
#include <cstring>

void swim_init(SWIMController* swim) {
    memset(swim, 0, sizeof(SWIMController));
}

void swim_reset(SWIMController* swim) {
    swim_init(swim);
}

uint8_t swim_read(SWIMController* swim, uint32_t offset) {
    // TODO: Implement IWM/SWIM register reads
    (void)swim; (void)offset;
    return 0;
}

void swim_write(SWIMController* swim, uint32_t offset, uint8_t val) {
    // TODO: Implement IWM/SWIM register writes
    (void)swim; (void)offset; (void)val;
}

bool swim_insert_disk(SWIMController* swim, const char* path, int drive) {
    (void)swim; (void)path; (void)drive;
    return false;
}

void swim_eject_disk(SWIMController* swim, int drive) {
    (void)swim; (void)drive;
}
