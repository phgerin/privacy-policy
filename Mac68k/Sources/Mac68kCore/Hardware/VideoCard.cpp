// VideoCard.cpp — Mac II built-in video card (NuBus)

#include "VideoCard.h"
#include <cstring>
#include <cstdlib>

// Declaration ROM — minimal NuBus ROM to identify the card
// The Mac II Slot Manager reads this at the top of slot space
static const uint8_t kDeclROM[] = {
    // TODO: Proper Mac II video declaration ROM
    // sRsrcType: catDisplay, typVideo
    // sRsrcName: "Display_Video_Apple_Mac2"
    // MinorBase/Length for VRAM
    0x00 // Placeholder
};

static uint8_t video_read_cb(NuBusCard* card, uint32_t offset);
static void video_write_cb(NuBusCard* card, uint32_t offset, uint8_t val);
static void video_reset_cb(NuBusCard* card);
static int video_irq_cb(NuBusCard* card);

VideoCard* video_create(void) {
    VideoCard* vc = (VideoCard*)calloc(1, sizeof(VideoCard));
    vc->depth = 8; // Default 8-bit color
    vc->width = VIDEO_WIDTH;
    vc->height = VIDEO_HEIGHT;

    // Set up NuBus card interface
    vc->card.read = video_read_cb;
    vc->card.write = video_write_cb;
    vc->card.reset = video_reset_cb;
    vc->card.irq_pending = video_irq_cb;
    vc->card.data = vc;

    // Initialize grayscale CLUT
    for (int i = 0; i < 256; i++) {
        vc->clut_r[i] = vc->clut_g[i] = vc->clut_b[i] = (uint8_t)i;
    }

    return vc;
}

void video_destroy(VideoCard* vc) {
    free(vc);
}

void video_reset(VideoCard* vc) {
    memset(vc->vram, 0, sizeof(vc->vram));
    vc->vbl_pending = 0;
    vc->clut_index = 0;
    vc->clut_phase = 0;
}

static void video_reset_cb(NuBusCard* card) {
    video_reset((VideoCard*)card->data);
}

static int video_irq_cb(NuBusCard* card) {
    VideoCard* vc = (VideoCard*)card->data;
    return vc->vbl_pending;
}

static uint8_t video_read_cb(NuBusCard* card, uint32_t offset) {
    VideoCard* vc = (VideoCard*)card->data;

    // VRAM is in the lower part of the slot space
    if (offset < sizeof(vc->vram)) {
        return vc->vram[offset];
    }

    // Declaration ROM at top of slot space ($FsFFxxxx)
    if (offset >= 0x00FF0000) {
        uint32_t rom_offset = offset - 0x00FF0000;
        if (rom_offset < sizeof(kDeclROM))
            return kDeclROM[rom_offset];
        return 0;
    }

    return 0xFF;
}

static void video_write_cb(NuBusCard* card, uint32_t offset, uint8_t val) {
    VideoCard* vc = (VideoCard*)card->data;

    // VRAM writes
    if (offset < sizeof(vc->vram)) {
        vc->vram[offset] = val;
        return;
    }

    // CLUT writes (RAMDAC registers)
    // TODO: Proper RAMDAC register addresses
}

void video_render_framebuffer(VideoCard* vc, uint32_t* argb_out) {
    // Convert indexed VRAM → ARGB8888 using CLUT
    switch (vc->depth) {
        case 8:
            for (int i = 0; i < vc->width * vc->height; i++) {
                uint8_t idx = vc->vram[i];
                argb_out[i] = 0xFF000000
                    | ((uint32_t)vc->clut_r[idx] << 16)
                    | ((uint32_t)vc->clut_g[idx] << 8)
                    | ((uint32_t)vc->clut_b[idx]);
            }
            break;
        case 4: {
            for (int i = 0; i < vc->width * vc->height; i++) {
                int byte_idx = i / 2;
                uint8_t idx = (i & 1) ? (vc->vram[byte_idx] & 0x0F)
                                       : (vc->vram[byte_idx] >> 4);
                idx *= 17; // Scale 0-15 → 0-255
                argb_out[i] = 0xFF000000
                    | ((uint32_t)vc->clut_r[idx] << 16)
                    | ((uint32_t)vc->clut_g[idx] << 8)
                    | ((uint32_t)vc->clut_b[idx]);
            }
            break;
        }
        case 1: {
            // 1-bit: black and white
            for (int i = 0; i < vc->width * vc->height; i++) {
                int byte_idx = i / 8;
                int bit = 7 - (i & 7);
                uint32_t pixel = (vc->vram[byte_idx] >> bit) & 1;
                // Mac convention: 0 = white, 1 = black
                argb_out[i] = pixel ? 0xFF000000 : 0xFFFFFFFF;
            }
            break;
        }
        default:
            // 2-bit: similar to 4-bit
            memset(argb_out, 0, vc->width * vc->height * sizeof(uint32_t));
            break;
    }
}
