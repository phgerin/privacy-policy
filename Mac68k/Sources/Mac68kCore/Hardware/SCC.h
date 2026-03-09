// SCC.h — Zilog 8530 Serial Communications Controller
//
// Mac II uses the SCC for serial ports (modem + printer)
// Also used for LocalTalk networking

#ifndef SCC_H
#define SCC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t rr[16];  // Read registers
    uint8_t wr[16];  // Write registers
    int reg_pointer;
} SCCChannel;

typedef struct {
    SCCChannel channel_a; // Modem port
    SCCChannel channel_b; // Printer port
} SCCController;

void scc_init(SCCController* scc);
void scc_reset(SCCController* scc);
uint8_t scc_read(SCCController* scc, uint32_t offset);
void scc_write(SCCController* scc, uint32_t offset, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif // SCC_H
