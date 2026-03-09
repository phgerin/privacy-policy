// MoiraBridge.h — Bridge between Moira C++ and C interface
// Subclass of moira::Moira implementing Mac II memory and I/O

#ifndef MOIRA_BRIDGE_H
#define MOIRA_BRIDGE_H

#ifdef __cplusplus

// Forward declaration — include actual Moira headers when sources are added
namespace moira {
    class Moira;
}

/// Mac II specific Moira subclass
/// Implements read/write callbacks with Mac II memory map
class MacCPU {
public:
    MacCPU();
    ~MacCPU();

    void reset();
    void executeFrame(int cycles_per_frame);
    void step();

    // Memory interface (called by Moira)
    uint8_t read8(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint32_t read32(uint32_t addr);
    void write8(uint32_t addr, uint8_t val);
    void write16(uint32_t addr, uint16_t val);
    void write32(uint32_t addr, uint32_t val);

    // Bus error support
    void raiseBusError(uint32_t addr);

    // Interrupt handling
    void setIPL(int level);

    // Register access
    uint32_t getD(int n) const;
    uint32_t getA(int n) const;
    uint32_t getPC() const;
    uint16_t getSR() const;
    uint32_t getVBR() const;

    void setBreakpoint(uint32_t addr);
    void clearBreakpoint(uint32_t addr);

private:
    // Moira instance will be here when sources are added
    // moira::Moira* cpu;
    int ipl_level = 0;
};

#endif // __cplusplus

#endif // MOIRA_BRIDGE_H
