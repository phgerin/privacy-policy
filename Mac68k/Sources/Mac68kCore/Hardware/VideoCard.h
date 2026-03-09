// VideoCard.h — Mac II built-in video card (NuBus slot $9)
//
// Mac II video: up to 640x480, 1/2/4/8-bit indexed color
// CLUT (Color Look-Up Table) via RAMDAC
// VRAM mapped in NuBus slot space

#ifndef VIDEO_CARD_H
#define VIDEO_CARD_H

#include <stdint.h>
#include "NuBus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_WIDTH     640
#define VIDEO_HEIGHT    480
#define VIDEO_MAX_DEPTH 8   // 8-bit = 256 colors

typedef struct {
    // VRAM (enough for 640x480x8bpp)
    uint8_t vram[VIDEO_WIDTH * VIDEO_HEIGHT];

    // CLUT — 256 entries, RGB888
    uint8_t clut_r[256];
    uint8_t clut_g[256];
    uint8_t clut_b[256];
    int clut_index;
    int clut_phase; // 0=R, 1=G, 2=B

    // Mode
    int depth;      // 1, 2, 4, or 8 bits per pixel
    int width;
    int height;

    // Interrupt
    int vbl_pending;

    // NuBus declaration ROM
    // (Minimal ROM to satisfy the Mac II slot manager)

    // NuBus card interface
    NuBusCard card;
} VideoCard;

VideoCard* video_create(void);
void video_destroy(VideoCard* vc);
void video_reset(VideoCard* vc);

// Convert indexed VRAM to ARGB8888 framebuffer for Metal
void video_render_framebuffer(VideoCard* vc, uint32_t* argb_out);

#ifdef __cplusplus
}
#endif

#endif // VIDEO_CARD_H
