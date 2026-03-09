// SCSIManager.cpp — NCR 5380 SCSI controller stub

#include "SCSIManager.h"
#include <cstring>

// NCR 5380 register offsets (Mac II: each register at offset * 0x200? verify)
enum NCR5380Reg {
    REG_DATA      = 0,
    REG_ICR       = 1,
    REG_MODE      = 2,
    REG_TCR       = 3,
    REG_STATUS    = 4, // read: bus status, write: select enable
    REG_DMA_STAT  = 5, // read: bus+status, write: start DMA send
    REG_INPUT     = 6, // read: input data, write: start DMA target recv
    REG_RESET_INT = 7  // read: reset int, write: start DMA init recv
};

void scsi_init(SCSIController* scsi) {
    memset(scsi, 0, sizeof(SCSIController));
}

void scsi_reset(SCSIController* scsi) {
    scsi_init(scsi);
}

uint8_t scsi_read(SCSIController* scsi, uint32_t offset) {
    uint8_t reg = (offset >> 4) & 0x07; // Verify register spacing

    switch (reg) {
        case REG_DATA:     return scsi->data;
        case REG_ICR:      return scsi->icr;
        case REG_MODE:     return scsi->mode;
        case REG_TCR:      return scsi->tcr;
        case REG_STATUS:   return scsi->bus_status;
        case REG_DMA_STAT: return scsi->status;
        case REG_INPUT:    return scsi->input_data;
        case REG_RESET_INT: return 0;
        default: return 0xFF;
    }
}

void scsi_write(SCSIController* scsi, uint32_t offset, uint8_t val) {
    uint8_t reg = (offset >> 4) & 0x07;

    switch (reg) {
        case REG_DATA:     scsi->data = val; break;
        case REG_ICR:      scsi->icr = val; break;
        case REG_MODE:     scsi->mode = val; break;
        case REG_TCR:      scsi->tcr = val & 0x07; break;
        case REG_STATUS:   scsi->sel_enable = val; break; // Select Enable (write)
        case REG_DMA_STAT: break; // Start DMA send
        case REG_INPUT:    break; // Start DMA target receive
        case REG_RESET_INT: scsi_reset(scsi); break;
    }
}

bool scsi_attach_image(SCSIController* scsi, int id, const char* path) {
    // TODO: Open disk image file, attach to SCSI target id
    (void)scsi; (void)id; (void)path;
    return false;
}

void scsi_detach_image(SCSIController* scsi, int id) {
    (void)scsi; (void)id;
}
