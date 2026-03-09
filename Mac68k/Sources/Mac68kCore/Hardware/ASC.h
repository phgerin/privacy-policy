// ASC.h — Apple Sound Chip
//
// Mac II uses the ASC (344S0063) for sound output
// Features: 4 voices wavetable synthesis, sample playback
// Base: $50014000, 0x800 bytes of wavetable RAM + registers

#ifndef ASC_H
#define ASC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASC_FIFO_SIZE   0x400   // 1K FIFO per channel (stereo)
#define ASC_WAVE_SIZE   0x800   // 2K wavetable RAM

typedef struct {
    uint8_t wavetable[ASC_WAVE_SIZE];
    uint8_t mode;        // 0=off, 1=FIFO, 2=wavetable
    uint8_t control;
    uint8_t volume;
    uint8_t fifo_left[ASC_FIFO_SIZE];
    uint8_t fifo_right[ASC_FIFO_SIZE];
    int fifo_left_pos;
    int fifo_right_pos;
    int fifo_left_count;
    int fifo_right_count;
    uint8_t irq_status;
} ASCChip;

void asc_init(ASCChip* asc);
void asc_reset(ASCChip* asc);
uint8_t asc_read(ASCChip* asc, uint32_t offset);
void asc_write(ASCChip* asc, uint32_t offset, uint8_t val);

// Generate audio samples (22255 Hz, signed 16-bit stereo)
void asc_generate(ASCChip* asc, int16_t* buffer, int sample_count);

#ifdef __cplusplus
}
#endif

#endif // ASC_H
