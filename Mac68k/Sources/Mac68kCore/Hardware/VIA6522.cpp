// VIA6522.cpp — MOS 6522 VIA emulation for Mac II

#include "VIA6522.h"
#include <cstring>

// VIA register offsets (each register is at offset * 0x200 on Mac II)
enum VIAReg {
    vBufB  = 0x00,  // Data Register B
    vBufA  = 0x01,  // Data Register A (with handshake)
    vDirB  = 0x02,  // Data Direction Register B
    vDirA  = 0x03,  // Data Direction Register A
    vT1CL  = 0x04,  // Timer 1 Counter Low
    vT1CH  = 0x05,  // Timer 1 Counter High
    vT1LL  = 0x06,  // Timer 1 Latch Low
    vT1LH  = 0x07,  // Timer 1 Latch High
    vT2CL  = 0x08,  // Timer 2 Counter Low
    vT2CH  = 0x09,  // Timer 2 Counter High
    vSR    = 0x0A,  // Shift Register
    vACR   = 0x0B,  // Auxiliary Control Register
    vPCR   = 0x0C,  // Peripheral Control Register
    vIFR   = 0x0D,  // Interrupt Flag Register
    vIER   = 0x0E,  // Interrupt Enable Register
    vBufDA = 0x0F   // Data Register A (no handshake)
};

void via_init(VIA6522* via) {
    memset(via, 0, sizeof(VIA6522));
}

void via_reset(VIA6522* via) {
    via_init(via);
}

static uint8_t via_reg_index(uint32_t offset) {
    // Mac II VIA registers are spaced 0x200 apart
    return (offset >> 9) & 0x0F;
}

uint8_t via_read(VIA6522* via, uint32_t offset) {
    uint8_t reg = via_reg_index(offset);

    switch (reg) {
        case vBufB:  return (via->orb & via->ddrb) | (via->irb & ~via->ddrb);
        case vBufA:
        case vBufDA: return (via->ora & via->ddra) | (via->ira & ~via->ddra);
        case vDirB:  return via->ddrb;
        case vDirA:  return via->ddra;
        case vT1CL:  via->ifr &= ~0x40; return via->t1c & 0xFF;
        case vT1CH:  return via->t1c >> 8;
        case vT1LL:  return via->t1l & 0xFF;
        case vT1LH:  return via->t1l >> 8;
        case vT2CL:  via->ifr &= ~0x20; return via->t2c & 0xFF;
        case vT2CH:  return via->t2c >> 8;
        case vSR:    return via->sr;
        case vACR:   return via->acr;
        case vPCR:   return via->pcr;
        case vIFR:   return via->ifr | (via_irq(via) ? 0x80 : 0x00);
        case vIER:   return via->ier | 0x80;
        default:     return 0xFF;
    }
}

void via_write(VIA6522* via, uint32_t offset, uint8_t val) {
    uint8_t reg = via_reg_index(offset);

    switch (reg) {
        case vBufB:  via->orb = val; break;
        case vBufA:
        case vBufDA: via->ora = val; break;
        case vDirB:  via->ddrb = val; break;
        case vDirA:  via->ddra = val; break;
        case vT1CL:  via->t1l = (via->t1l & 0xFF00) | val; break;
        case vT1CH:
            via->t1l = (via->t1l & 0x00FF) | ((uint16_t)val << 8);
            via->t1c = via->t1l;
            via->ifr &= ~0x40;
            break;
        case vT1LL:  via->t1l = (via->t1l & 0xFF00) | val; break;
        case vT1LH:  via->t1l = (via->t1l & 0x00FF) | ((uint16_t)val << 8); break;
        case vT2CL:  via->t2l = (via->t2l & 0xFF00) | val; break;
        case vT2CH:
            via->t2c = ((uint16_t)val << 8) | (via->t2l & 0xFF);
            via->ifr &= ~0x20;
            break;
        case vSR:    via->sr = val; break;
        case vACR:   via->acr = val; break;
        case vPCR:   via->pcr = val; break;
        case vIFR:   via->ifr &= ~(val & 0x7F); break;
        case vIER:
            if (val & 0x80)
                via->ier |= (val & 0x7F);
            else
                via->ier &= ~(val & 0x7F);
            break;
    }
}

void via_tick(VIA6522* via, int cycles) {
    // Timer 1
    int old_t1 = via->t1c;
    via->t1c -= cycles;
    if (old_t1 > 0 && via->t1c <= 0) {
        via->ifr |= 0x40; // Timer 1 interrupt
        if (via->acr & 0x40) {
            // Free-running mode: reload from latch
            via->t1c += via->t1l;
        }
    }

    // Timer 2 (one-shot only in timed mode)
    if (!(via->acr & 0x20)) {
        int old_t2 = via->t2c;
        via->t2c -= cycles;
        if (old_t2 > 0 && via->t2c <= 0) {
            via->ifr |= 0x20; // Timer 2 interrupt
        }
    }
}

int via_irq(VIA6522* via) {
    return (via->ifr & via->ier & 0x7F) != 0;
}
