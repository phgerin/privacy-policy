# Moira 68020 CPU Core

Copy the Moira source files here from:
https://github.com/dirkwhoffmann/Moira

Required files:
- Moira.h / Moira.cpp
- MoiraConfig.h
- MoiraTypes.h
- MoiraDebugger.h / MoiraDebugger.cpp
- And all supporting headers/sources

## Configuration

In MoiraConfig.h, ensure:
- `MOIRA_EMULATE_FC` is enabled (function codes for Mac II MMU)
- `MOIRA_ENABLE_BUS_ERROR` is enabled
- CPU model is set to M68020

## Bus Error Notes

Bus errors are critical for Mac II emulation:
- ROM uses bus errors for memory sizing at startup
- PMMU uses bus errors for page faults
- NuBus slot detection probes via bus errors
- I/O space access validation

Philippe has contributed bus error fixes to Moira v3 — use the latest version.
