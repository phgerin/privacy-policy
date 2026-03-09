// NuBus.cpp — NuBus slot manager

#include "NuBus.h"
#include <cstring>

void nubus_init(NuBusManager* nubus) {
    memset(nubus->slots, 0, sizeof(nubus->slots));
}

void nubus_reset(NuBusManager* nubus) {
    for (int i = 0; i < NUBUS_NUM_SLOTS; i++) {
        if (nubus->slots[i] && nubus->slots[i]->reset) {
            nubus->slots[i]->reset(nubus->slots[i]);
        }
    }
}

bool nubus_install(NuBusManager* nubus, int slot_id, NuBusCard* card) {
    if (slot_id < NUBUS_SLOT_MIN || slot_id > NUBUS_SLOT_MAX) return false;
    int index = slot_id - NUBUS_SLOT_MIN;
    nubus->slots[index] = card;
    return true;
}

void nubus_remove(NuBusManager* nubus, int slot_id) {
    if (slot_id < NUBUS_SLOT_MIN || slot_id > NUBUS_SLOT_MAX) return;
    nubus->slots[slot_id - NUBUS_SLOT_MIN] = NULL;
}

static int slot_from_addr(uint32_t addr) {
    // Both super ($6s) and standard ($Fs) spaces use nibble for slot
    int nibble = (addr >> 24) & 0x0F;
    if (nibble >= NUBUS_SLOT_MIN && nibble <= NUBUS_SLOT_MAX)
        return nibble;
    return -1;
}

uint8_t nubus_read(NuBusManager* nubus, uint32_t addr) {
    int slot = slot_from_addr(addr);
    if (slot < 0) return 0xFF;

    NuBusCard* card = nubus->slots[slot - NUBUS_SLOT_MIN];
    if (!card) return 0xFF; // No card → should trigger bus error

    uint32_t offset = addr & 0x00FFFFFF; // 16 MB slot space
    return card->read(card, offset);
}

void nubus_write(NuBusManager* nubus, uint32_t addr, uint8_t val) {
    int slot = slot_from_addr(addr);
    if (slot < 0) return;

    NuBusCard* card = nubus->slots[slot - NUBUS_SLOT_MIN];
    if (!card) return; // No card → should trigger bus error

    uint32_t offset = addr & 0x00FFFFFF;
    card->write(card, offset, val);
}

uint8_t nubus_irq_status(NuBusManager* nubus) {
    uint8_t status = 0;
    for (int i = 0; i < NUBUS_NUM_SLOTS; i++) {
        if (nubus->slots[i] && nubus->slots[i]->irq_pending &&
            nubus->slots[i]->irq_pending(nubus->slots[i])) {
            status |= (1 << i);
        }
    }
    return status;
}
