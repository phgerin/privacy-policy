// MemoryManager.h — Mac II Memory Map
//
// Mac II Memory Map (32-bit clean):
//
// $00000000-$3FFFFFFF  RAM (up to 128 MB, typically 1-8 MB)
// $40000000-$4FFFFFFF  ROM (256K or 512K, mirrored)
//   $40800000           Mac II ROM base
// $50000000-$5FFFFFFF  I/O Space
//   $50000000           VIA1 (6522)
//   $50002000           VIA2 (6522)
//   $50004000           SCC Read
//   $50006000           SCC Write (no: this is a stub - need to verify)
//   $50012000           SCSI (NCR 5380)
//   $50014000           ASC (Apple Sound Chip)
//   $50016000           SWIM (floppy)
//   $50024000           SCC DMA (if present)
//   $50040000           PMMU (68851) on early Mac II
// $60000000-$6FFFFFFF  NuBus super slot space
// $F0000000-$FFFFFFFF  NuBus standard slot space ($Fs000000 for slot s)
//   $F9000000           Slot $9 (built-in video on Mac IIci)

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// RAM
#define MAC_II_MAX_RAM      (8 * 1024 * 1024)   // 8 MB default
#define MAC_II_ROM_SIZE     (256 * 1024)          // 256K ROM

// Memory regions
#define MAC_II_RAM_BASE     0x00000000
#define MAC_II_ROM_BASE     0x40800000
#define MAC_II_IO_BASE      0x50000000
#define MAC_II_NUBUS_SUPER  0x60000000
#define MAC_II_NUBUS_STD    0xF0000000

// I/O offsets from IO_BASE
#define IO_VIA1             0x00000000
#define IO_VIA2             0x00002000
#define IO_SCC_READ         0x00004000
#define IO_SCC_WRITE        0x00006000 // Verify against GMII docs
#define IO_SCSI             0x00012000
#define IO_ASC              0x00014000
#define IO_SWIM             0x00016000

#ifdef __cplusplus
}
#endif

#endif // MEMORY_MANAGER_H
