// Mac68kCore.h — Public C interface for Swift interop
// Macintosh II emulator based on Moira 68020

#ifndef MAC68K_CORE_H
#define MAC68K_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// MARK: - Emulator lifecycle

typedef struct Mac68kEmulator Mac68kEmulator;

Mac68kEmulator* mac68k_create(void);
void mac68k_destroy(Mac68kEmulator* emu);

// MARK: - ROM and disk

bool mac68k_load_rom(Mac68kEmulator* emu, const char* path);
bool mac68k_insert_disk(Mac68kEmulator* emu, const char* path, int drive);
void mac68k_eject_disk(Mac68kEmulator* emu, int drive);

// MARK: - Emulation control

void mac68k_reset(Mac68kEmulator* emu);
void mac68k_run_frame(Mac68kEmulator* emu);
void mac68k_pause(Mac68kEmulator* emu);
void mac68k_resume(Mac68kEmulator* emu);
bool mac68k_is_running(Mac68kEmulator* emu);

// MARK: - Video output

/// Returns pointer to the framebuffer (ARGB8888, 640x480 max)
const uint32_t* mac68k_get_framebuffer(Mac68kEmulator* emu);
int mac68k_get_screen_width(Mac68kEmulator* emu);
int mac68k_get_screen_height(Mac68kEmulator* emu);

// MARK: - Input

void mac68k_mouse_move(Mac68kEmulator* emu, int dx, int dy);
void mac68k_mouse_button(Mac68kEmulator* emu, bool down);
void mac68k_key_event(Mac68kEmulator* emu, uint8_t keycode, bool down);

// MARK: - Audio

/// Returns pointer to audio sample buffer (signed 16-bit stereo, 22255 Hz)
const int16_t* mac68k_get_audio_buffer(Mac68kEmulator* emu, int* sample_count);

// MARK: - Debugger

typedef struct {
    uint32_t d[8];     // Data registers
    uint32_t a[8];     // Address registers (A7 = SSP or USP)
    uint32_t pc;       // Program counter
    uint16_t sr;       // Status register
    uint32_t usp;      // User stack pointer
    uint32_t ssp;      // Supervisor stack pointer
    uint32_t vbr;      // Vector base register (68020)
    uint32_t cacr;     // Cache control register (68020)
    uint32_t caar;     // Cache address register (68020)
} Mac68kRegisters;

void mac68k_get_registers(Mac68kEmulator* emu, Mac68kRegisters* regs);
void mac68k_set_breakpoint(Mac68kEmulator* emu, uint32_t address);
void mac68k_clear_breakpoint(Mac68kEmulator* emu, uint32_t address);
void mac68k_step(Mac68kEmulator* emu);

// MARK: - Interrupt control

void mac68k_set_irq(Mac68kEmulator* emu, int level);

#ifdef __cplusplus
}
#endif

#endif // MAC68K_CORE_H
