// ASC.cpp — Apple Sound Chip stub

#include "ASC.h"
#include <cstring>

// ASC register offsets
enum ASCReg {
    ASC_VERSION  = 0x800,
    ASC_MODE     = 0x801,
    ASC_CONTROL  = 0x802,
    ASC_FIFO_IRQ = 0x803,
    ASC_FIFO_CTL = 0x804,
    ASC_VOLUME   = 0x806
};

void asc_init(ASCChip* asc) {
    memset(asc, 0, sizeof(ASCChip));
}

void asc_reset(ASCChip* asc) {
    asc_init(asc);
}

uint8_t asc_read(ASCChip* asc, uint32_t offset) {
    if (offset < ASC_WAVE_SIZE) {
        return asc->wavetable[offset];
    }
    switch (offset) {
        case ASC_VERSION:  return 0x00; // Mac II ASC version
        case ASC_MODE:     return asc->mode;
        case ASC_CONTROL:  return asc->control;
        case ASC_FIFO_IRQ: return asc->irq_status;
        case ASC_VOLUME:   return asc->volume;
        default: return 0;
    }
}

void asc_write(ASCChip* asc, uint32_t offset, uint8_t val) {
    if (offset < ASC_WAVE_SIZE) {
        asc->wavetable[offset] = val;
        return;
    }
    switch (offset) {
        case ASC_MODE:
            asc->mode = val;
            if (val == 0) asc_reset(asc); // Mode 0 = clear
            break;
        case ASC_CONTROL:
            asc->control = val;
            break;
        case ASC_FIFO_IRQ:
            asc->irq_status &= ~val; // Write 1 to clear
            break;
        case ASC_VOLUME:
            asc->volume = val;
            break;
    }
}

void asc_generate(ASCChip* asc, int16_t* buffer, int sample_count) {
    // TODO: Generate audio from FIFO or wavetable
    memset(buffer, 0, sample_count * 2 * sizeof(int16_t));
}
