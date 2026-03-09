// MoiraBridge.cpp — Moira integration for Mac II emulation
//
// This file implements:
// 1. The MacCPU class wrapping Moira
// 2. The C interface (Mac68kCore.h) for Swift interop
// 3. Memory-mapped I/O dispatch to hardware modules

#include "MoiraBridge.h"
#include "Mac68kCore.h"
// #include "Moira.h"  // Uncomment when Moira sources are added
// #include "MemoryManager.h"

#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
// Mac II constants
// ---------------------------------------------------------------------------

static constexpr int kCyclesPerFrame = 16625 * 60; // ~16 MHz 68020, 60 Hz

// Framebuffer (placeholder: 640x480 8-bit indexed → ARGB8888 for Metal)
static constexpr int kMaxScreenWidth = 640;
static constexpr int kMaxScreenHeight = 480;

// ---------------------------------------------------------------------------
// MacCPU implementation (stub until Moira sources are added)
// ---------------------------------------------------------------------------

MacCPU::MacCPU() {
    // TODO: Instantiate Moira in 68020 mode
    // cpu = new moira::Moira();
    // cpu->setModel(moira::M68020);
}

MacCPU::~MacCPU() {
    // delete cpu;
}

void MacCPU::reset() {
    // cpu->reset();
}

void MacCPU::executeFrame(int cycles_per_frame) {
    // int remaining = cycles_per_frame;
    // while (remaining > 0) {
    //     remaining -= cpu->execute();
    // }
}

void MacCPU::step() {
    // cpu->execute();  // Execute one instruction
}

void MacCPU::setIPL(int level) {
    ipl_level = level;
    // cpu->setIPL(level);
}

void MacCPU::raiseBusError(uint32_t addr) {
    // cpu->signalBusError();
}

uint32_t MacCPU::getD(int n) const { return 0; /* cpu->getD(n); */ }
uint32_t MacCPU::getA(int n) const { return 0; /* cpu->getA(n); */ }
uint32_t MacCPU::getPC() const { return 0; /* cpu->getPC(); */ }
uint16_t MacCPU::getSR() const { return 0; /* cpu->getSR(); */ }
uint32_t MacCPU::getVBR() const { return 0; /* cpu->getVBR(); */ }

void MacCPU::setBreakpoint(uint32_t addr) {
    // cpu->setBreakpoint(addr);
}

void MacCPU::clearBreakpoint(uint32_t addr) {
    // cpu->clearBreakpoint(addr);
}

// ---------------------------------------------------------------------------
// Mac68kEmulator — top-level emulator aggregating all hardware
// ---------------------------------------------------------------------------

struct Mac68kEmulator {
    MacCPU cpu;
    uint32_t framebuffer[kMaxScreenWidth * kMaxScreenHeight];
    int screen_width = kMaxScreenWidth;
    int screen_height = kMaxScreenHeight;
    bool running = false;
    int16_t audio_buffer[22255 * 2]; // 1 second stereo at Mac II sample rate
    int audio_samples = 0;
};

// ---------------------------------------------------------------------------
// C interface implementation
// ---------------------------------------------------------------------------

extern "C" {

Mac68kEmulator* mac68k_create(void) {
    auto* emu = new Mac68kEmulator();
    memset(emu->framebuffer, 0, sizeof(emu->framebuffer));
    return emu;
}

void mac68k_destroy(Mac68kEmulator* emu) {
    delete emu;
}

bool mac68k_load_rom(Mac68kEmulator* emu, const char* path) {
    // TODO: Load Mac II ROM (256K or 512K)
    // Validate checksum
    // Map to ROM address space ($40800000 for Mac II)
    (void)emu; (void)path;
    return false;
}

bool mac68k_insert_disk(Mac68kEmulator* emu, const char* path, int drive) {
    // TODO: Mount disk image (400K, 800K, 1.4M floppy or SCSI HD image)
    (void)emu; (void)path; (void)drive;
    return false;
}

void mac68k_eject_disk(Mac68kEmulator* emu, int drive) {
    (void)emu; (void)drive;
}

void mac68k_reset(Mac68kEmulator* emu) {
    emu->cpu.reset();
    emu->running = true;
}

void mac68k_run_frame(Mac68kEmulator* emu) {
    if (!emu->running) return;
    emu->cpu.executeFrame(kCyclesPerFrame);
}

void mac68k_pause(Mac68kEmulator* emu) {
    emu->running = false;
}

void mac68k_resume(Mac68kEmulator* emu) {
    emu->running = true;
}

bool mac68k_is_running(Mac68kEmulator* emu) {
    return emu->running;
}

const uint32_t* mac68k_get_framebuffer(Mac68kEmulator* emu) {
    return emu->framebuffer;
}

int mac68k_get_screen_width(Mac68kEmulator* emu) {
    return emu->screen_width;
}

int mac68k_get_screen_height(Mac68kEmulator* emu) {
    return emu->screen_height;
}

void mac68k_mouse_move(Mac68kEmulator* emu, int dx, int dy) {
    // TODO: Forward to ADB mouse
    (void)emu; (void)dx; (void)dy;
}

void mac68k_mouse_button(Mac68kEmulator* emu, bool down) {
    // TODO: Forward to ADB mouse
    (void)emu; (void)down;
}

void mac68k_key_event(Mac68kEmulator* emu, uint8_t keycode, bool down) {
    // TODO: Forward to ADB keyboard
    (void)emu; (void)keycode; (void)down;
}

const int16_t* mac68k_get_audio_buffer(Mac68kEmulator* emu, int* sample_count) {
    *sample_count = emu->audio_samples;
    return emu->audio_buffer;
}

void mac68k_get_registers(Mac68kEmulator* emu, Mac68kRegisters* regs) {
    for (int i = 0; i < 8; i++) {
        regs->d[i] = emu->cpu.getD(i);
        regs->a[i] = emu->cpu.getA(i);
    }
    regs->pc = emu->cpu.getPC();
    regs->sr = emu->cpu.getSR();
    regs->vbr = emu->cpu.getVBR();
}

void mac68k_set_breakpoint(Mac68kEmulator* emu, uint32_t address) {
    emu->cpu.setBreakpoint(address);
}

void mac68k_clear_breakpoint(Mac68kEmulator* emu, uint32_t address) {
    emu->cpu.clearBreakpoint(address);
}

void mac68k_step(Mac68kEmulator* emu) {
    emu->cpu.step();
}

void mac68k_set_irq(Mac68kEmulator* emu, int level) {
    emu->cpu.setIPL(level);
}

} // extern "C"
