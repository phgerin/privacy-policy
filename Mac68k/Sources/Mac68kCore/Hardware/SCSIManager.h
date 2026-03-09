// SCSIManager.h — NCR 5380 SCSI controller for Mac II

#ifndef SCSI_MANAGER_H
#define SCSI_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// NCR 5380 registers
typedef struct {
    uint8_t data;           // Current SCSI Data
    uint8_t icr;            // Initiator Command Register
    uint8_t mode;           // Mode Register
    uint8_t tcr;            // Target Command Register
    uint8_t bus_status;     // Current Bus Status (read)
    uint8_t sel_enable;     // Select Enable (write)
    uint8_t status;         // Bus and Status (read)
    uint8_t start_dma;      // Start DMA Send/Recv (write)
    uint8_t input_data;     // Input Data (read)
    uint8_t reset;          // Reset (write)

    // Emulator state
    int phase;
    int target_id;
    bool connected;
} SCSIController;

void scsi_init(SCSIController* scsi);
void scsi_reset(SCSIController* scsi);
uint8_t scsi_read(SCSIController* scsi, uint32_t offset);
void scsi_write(SCSIController* scsi, uint32_t offset, uint8_t val);

// Disk image interface
bool scsi_attach_image(SCSIController* scsi, int id, const char* path);
void scsi_detach_image(SCSIController* scsi, int id);

#ifdef __cplusplus
}
#endif

#endif // SCSI_MANAGER_H
