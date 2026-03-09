// SCC.cpp — Zilog 8530 SCC stub

#include "SCC.h"
#include <cstring>

void scc_init(SCCController* scc) {
    memset(scc, 0, sizeof(SCCController));
    // RR0 defaults: Tx buffer empty, no chars available
    scc->channel_a.rr[0] = 0x04; // Tx empty
    scc->channel_b.rr[0] = 0x04;
}

void scc_reset(SCCController* scc) {
    scc_init(scc);
}

uint8_t scc_read(SCCController* scc, uint32_t offset) {
    // Mac II SCC addressing:
    // Bit 1 selects channel (0=B, 1=A)
    // Bit 0 selects control(0) vs data(1)
    int channel_a = (offset >> 1) & 1;
    int is_data = offset & 1;
    SCCChannel* ch = channel_a ? &scc->channel_a : &scc->channel_b;

    if (is_data) {
        return 0; // No data available
    }

    uint8_t val = ch->rr[ch->reg_pointer];
    ch->reg_pointer = 0; // Reset to RR0 after read
    return val;
}

void scc_write(SCCController* scc, uint32_t offset, uint8_t val) {
    int channel_a = (offset >> 1) & 1;
    int is_data = offset & 1;
    SCCChannel* ch = channel_a ? &scc->channel_a : &scc->channel_b;

    if (is_data) {
        // TODO: Handle serial output
        return;
    }

    if (ch->reg_pointer == 0) {
        // WR0: register pointer and commands
        uint8_t cmd = (val >> 3) & 0x07;
        ch->reg_pointer = val & 0x07;

        switch (cmd) {
            case 2: // Reset ext/status interrupts
                break;
            case 3: // Send abort
                break;
            case 6: // Reset Rx CRC
                break;
            case 7: // Reset Tx CRC
                break;
        }
    } else {
        ch->wr[ch->reg_pointer] = val;
        ch->reg_pointer = 0;
    }
}
