// DebuggerView.swift — CPU register inspector and step debugger

import SwiftUI

struct DebuggerView: View {
    @EnvironmentObject var emulator: EmulatorViewModel

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Text("Debugger")
                .font(.headline)

            // Data registers
            GroupBox("Data Registers") {
                LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible())],
                          spacing: 4) {
                    ForEach(0..<8, id: \.self) { i in
                        HStack {
                            Text("D\(i)")
                                .font(.system(.caption, design: .monospaced))
                                .foregroundColor(.secondary)
                            Text(hex32(regD(i)))
                                .font(.system(.caption, design: .monospaced))
                        }
                    }
                }
            }

            // Address registers
            GroupBox("Address Registers") {
                LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible())],
                          spacing: 4) {
                    ForEach(0..<8, id: \.self) { i in
                        HStack {
                            Text("A\(i)")
                                .font(.system(.caption, design: .monospaced))
                                .foregroundColor(.secondary)
                            Text(hex32(regA(i)))
                                .font(.system(.caption, design: .monospaced))
                        }
                    }
                }
            }

            // Special registers
            GroupBox("Control") {
                VStack(alignment: .leading, spacing: 4) {
                    registerRow("PC ", emulator.registers.pc)
                    registerRow("SR ", UInt32(emulator.registers.sr))
                    registerRow("VBR", emulator.registers.vbr)
                }
                .font(.system(.caption, design: .monospaced))
            }

            // Controls
            HStack {
                Button("Step") {
                    emulator.stepInstruction()
                }
                .keyboardShortcut("S", modifiers: [.control])

                Button("Continue") {
                    emulator.togglePause()
                }

                Spacer()

                Button("Refresh") {
                    emulator.updateRegisters()
                }
            }
            .buttonStyle(.bordered)

            Spacer()
        }
        .padding()
    }

    private func hex32(_ val: UInt32) -> String {
        String(format: "$%08X", val)
    }

    private func registerRow(_ name: String, _ val: UInt32) -> some View {
        HStack {
            Text(name)
                .foregroundColor(.secondary)
            Text(hex32(val))
        }
    }

    private func regD(_ i: Int) -> UInt32 {
        withUnsafePointer(to: emulator.registers.d) { ptr in
            ptr.withMemoryRebound(to: UInt32.self, capacity: 8) { $0[i] }
        }
    }

    private func regA(_ i: Int) -> UInt32 {
        withUnsafePointer(to: emulator.registers.a) { ptr in
            ptr.withMemoryRebound(to: UInt32.self, capacity: 8) { $0[i] }
        }
    }
}
