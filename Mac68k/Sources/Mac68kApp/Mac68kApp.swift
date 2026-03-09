// Mac68kApp.swift — Macintosh II Emulator
// SwiftUI + Metal frontend, Moira 68020 CPU core

import SwiftUI

@main
struct Mac68kApp: App {
    @StateObject private var emulator = EmulatorViewModel()

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(emulator)
        }
        .commands {
            FileCommands(emulator: emulator)
            EmulatorCommands(emulator: emulator)
        }

        Settings {
            SettingsView()
                .environmentObject(emulator)
        }
    }
}

// MARK: - Menu commands

struct FileCommands: Commands {
    @ObservedObject var emulator: EmulatorViewModel

    var body: some Commands {
        CommandGroup(after: .newItem) {
            Button("Open ROM...") {
                emulator.openROM()
            }
            .keyboardShortcut("O", modifiers: [.command])

            Divider()

            Button("Insert Floppy Disk...") {
                emulator.insertDisk(drive: 0)
            }
            .keyboardShortcut("D", modifiers: [.command])

            Button("Mount SCSI Disk...") {
                emulator.mountSCSIDisk()
            }
            .keyboardShortcut("H", modifiers: [.command, .shift])
        }
    }
}

struct EmulatorCommands: Commands {
    @ObservedObject var emulator: EmulatorViewModel

    var body: some Commands {
        CommandMenu("Machine") {
            Button("Reset") {
                emulator.reset()
            }
            .keyboardShortcut("R", modifiers: [.command, .shift])

            Button(emulator.isRunning ? "Pause" : "Resume") {
                emulator.togglePause()
            }
            .keyboardShortcut("P", modifiers: [.command])

            Divider()

            Button("Debugger") {
                emulator.showDebugger.toggle()
            }
            .keyboardShortcut("B", modifiers: [.command, .option])
        }
    }
}
