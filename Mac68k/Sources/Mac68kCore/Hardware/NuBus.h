// NuBus.h — NuBus expansion slot manager
//
// Mac II has 6 NuBus slots ($9 through $E)
// Slot $9 is often the built-in video card
// Each slot has:
//   Super slot space: $6s000000-$6sFFFFFF (16 MB per slot)
//   Standard slot space: $Fs000000-$FsFFFFFF (16 MB per slot)
//   Declaration ROM at top of standard slot space

#ifndef NUBUS_H
#define NUBUS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUBUS_SLOT_MIN  0x9
#define NUBUS_SLOT_MAX  0xE
#define NUBUS_NUM_SLOTS 6

typedef struct NuBusCard NuBusCard;

// Card interface — each card type implements these
struct NuBusCard {
    uint8_t (*read)(NuBusCard* card, uint32_t offset);
    void (*write)(NuBusCard* card, uint32_t offset, uint8_t val);
    void (*reset)(NuBusCard* card);
    int (*irq_pending)(NuBusCard* card);
    void* data; // Card-specific data
};

typedef struct {
    NuBusCard* slots[NUBUS_NUM_SLOTS]; // slots[0] = slot $9, etc.
} NuBusManager;

void nubus_init(NuBusManager* nubus);
void nubus_reset(NuBusManager* nubus);

// Install a card in a slot (slot_id: 0x9..0xE)
bool nubus_install(NuBusManager* nubus, int slot_id, NuBusCard* card);
void nubus_remove(NuBusManager* nubus, int slot_id);

// Memory access (addr is full 32-bit address)
uint8_t nubus_read(NuBusManager* nubus, uint32_t addr);
void nubus_write(NuBusManager* nubus, uint32_t addr, uint8_t val);

// Returns combined IRQ status for VIA2 slot interrupt routing
uint8_t nubus_irq_status(NuBusManager* nubus);

#ifdef __cplusplus
}
#endif

#endif // NUBUS_H
