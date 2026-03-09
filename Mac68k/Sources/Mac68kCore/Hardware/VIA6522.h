// VIA6522.h — MOS 6522 Versatile Interface Adapter
//
// Mac II has two VIAs:
// VIA1: System control — keyboard/mouse clock, RTC, sound volume,
//       overlay bit, interrupt routing
// VIA2: NuBus/SCSI interrupts, slot IRQ routing, power management

#ifndef VIA6522_H
#define VIA6522_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // Registers
    uint8_t ora;     // Output Register A (active)
    uint8_t orb;     // Output Register B (active)
    uint8_t ira;     // Input Register A
    uint8_t irb;     // Input Register B
    uint8_t ddra;    // Data Direction Register A
    uint8_t ddrb;    // Data Direction Register B
    uint16_t t1c;    // Timer 1 counter
    uint16_t t1l;    // Timer 1 latch
    uint16_t t2c;    // Timer 2 counter
    uint16_t t2l;    // Timer 2 latch (low byte only)
    uint8_t sr;      // Shift Register
    uint8_t acr;     // Auxiliary Control Register
    uint8_t pcr;     // Peripheral Control Register
    uint8_t ifr;     // Interrupt Flag Register
    uint8_t ier;     // Interrupt Enable Register
} VIA6522;

void via_init(VIA6522* via);
void via_reset(VIA6522* via);
uint8_t via_read(VIA6522* via, uint32_t offset);
void via_write(VIA6522* via, uint32_t offset, uint8_t val);
void via_tick(VIA6522* via, int cycles);

// Returns true if VIA is asserting an interrupt
int via_irq(VIA6522* via);

#ifdef __cplusplus
}
#endif

#endif // VIA6522_H
