// MemoryManager.cpp — Mac II memory map dispatcher
//
// Dispatches CPU memory accesses to the correct hardware module
// based on the Mac II address map.

#include "MemoryManager.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

static uint8_t ram[MAC_II_MAX_RAM];
static uint8_t rom[MAC_II_ROM_SIZE];
static uint32_t ram_size = MAC_II_MAX_RAM;
static bool rom_loaded = false;

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void memory_init(uint32_t ram_bytes) {
    ram_size = ram_bytes;
    memset(ram, 0, sizeof(ram));
    memset(rom, 0, sizeof(rom));
    rom_loaded = false;
}

bool memory_load_rom(const uint8_t* data, uint32_t size) {
    if (size > MAC_II_ROM_SIZE) return false;
    memcpy(rom, data, size);
    rom_loaded = true;
    return true;
}

// ---------------------------------------------------------------------------
// Address decoding
// ---------------------------------------------------------------------------

typedef enum {
    REGION_RAM,
    REGION_ROM,
    REGION_VIA1,
    REGION_VIA2,
    REGION_SCC,
    REGION_SCSI,
    REGION_ASC,
    REGION_SWIM,
    REGION_NUBUS,
    REGION_BUS_ERROR  // Unmapped — triggers bus error
} MemRegion;

static MemRegion decode_address(uint32_t addr, uint32_t* offset) {
    if (addr < ram_size) {
        *offset = addr;
        return REGION_RAM;
    }
    if (addr < 0x40000000) {
        // RAM space but beyond installed RAM → bus error
        // This is how the ROM detects installed memory size
        return REGION_BUS_ERROR;
    }
    if (addr >= MAC_II_ROM_BASE && addr < MAC_II_ROM_BASE + MAC_II_ROM_SIZE) {
        *offset = addr - MAC_II_ROM_BASE;
        return REGION_ROM;
    }
    if ((addr & 0xF0000000) == MAC_II_IO_BASE) {
        uint32_t io_offset = addr - MAC_II_IO_BASE;
        if (io_offset < IO_VIA1 + 0x2000) { *offset = io_offset; return REGION_VIA1; }
        if (io_offset < IO_VIA2 + 0x2000) { *offset = io_offset - IO_VIA2; return REGION_VIA2; }
        if (io_offset < IO_SCC_READ + 0x2000) { *offset = io_offset - IO_SCC_READ; return REGION_SCC; }
        if (io_offset < IO_SCC_WRITE + 0x2000) { *offset = io_offset - IO_SCC_WRITE; return REGION_SCC; }
        if (io_offset >= IO_SCSI && io_offset < IO_SCSI + 0x2000) { *offset = io_offset - IO_SCSI; return REGION_SCSI; }
        if (io_offset >= IO_ASC && io_offset < IO_ASC + 0x2000) { *offset = io_offset - IO_ASC; return REGION_ASC; }
        if (io_offset >= IO_SWIM && io_offset < IO_SWIM + 0x2000) { *offset = io_offset - IO_SWIM; return REGION_SWIM; }
        return REGION_BUS_ERROR;
    }
    if ((addr & 0xF0000000) == MAC_II_NUBUS_SUPER ||
        (addr & 0xF0000000) == MAC_II_NUBUS_STD) {
        *offset = addr;
        return REGION_NUBUS;
    }
    return REGION_BUS_ERROR;
}

// ---------------------------------------------------------------------------
// Read / Write (called from MoiraBridge)
// ---------------------------------------------------------------------------

uint8_t memory_read8(uint32_t addr) {
    uint32_t offset = 0;
    MemRegion region = decode_address(addr, &offset);

    switch (region) {
        case REGION_RAM:
            return ram[offset];
        case REGION_ROM:
            return rom[offset];
        case REGION_VIA1:
            // TODO: via1_read(offset)
            return 0;
        case REGION_VIA2:
            // TODO: via2_read(offset)
            return 0;
        case REGION_SCC:
            // TODO: scc_read(offset)
            return 0;
        case REGION_SCSI:
            // TODO: scsi_read(offset)
            return 0;
        case REGION_ASC:
            // TODO: asc_read(offset)
            return 0;
        case REGION_SWIM:
            // TODO: swim_read(offset)
            return 0;
        case REGION_NUBUS:
            // TODO: nubus_read(offset)
            return 0xFF; // No card → bus error in real hardware
        case REGION_BUS_ERROR:
            // TODO: Signal bus error to Moira
            // cpu->raiseBusError(addr);
            return 0xFF;
    }
    return 0xFF;
}

uint16_t memory_read16(uint32_t addr) {
    return ((uint16_t)memory_read8(addr) << 8) | memory_read8(addr + 1);
}

void memory_write8(uint32_t addr, uint8_t val) {
    uint32_t offset = 0;
    MemRegion region = decode_address(addr, &offset);

    switch (region) {
        case REGION_RAM:
            ram[offset] = val;
            break;
        case REGION_ROM:
            // Write to ROM → ignored (or bus error on real hardware)
            break;
        case REGION_VIA1:
            // TODO: via1_write(offset, val)
            break;
        case REGION_VIA2:
            // TODO: via2_write(offset, val)
            break;
        case REGION_SCC:
            // TODO: scc_write(offset, val)
            break;
        case REGION_SCSI:
            // TODO: scsi_write(offset, val)
            break;
        case REGION_ASC:
            // TODO: asc_write(offset, val)
            break;
        case REGION_SWIM:
            // TODO: swim_write(offset, val)
            break;
        case REGION_NUBUS:
            // TODO: nubus_write(offset, val)
            break;
        case REGION_BUS_ERROR:
            // TODO: Signal bus error to Moira
            break;
    }
}

void memory_write16(uint32_t addr, uint16_t val) {
    memory_write8(addr, (uint8_t)(val >> 8));
    memory_write8(addr + 1, (uint8_t)(val & 0xFF));
}
