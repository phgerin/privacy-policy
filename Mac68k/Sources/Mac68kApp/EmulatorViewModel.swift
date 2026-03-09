// EmulatorViewModel.swift — Bridge between C emulator core and SwiftUI

import SwiftUI
import Combine
import Mac68kCore

@MainActor
class EmulatorViewModel: ObservableObject {
    @Published var isRunning = false
    @Published var showDebugger = false
    @Published var screenWidth = 640
    @Published var screenHeight = 480
    @Published var statusMessage = "No ROM loaded"
    @Published var registers = Mac68kRegisters()

    private var emulator: OpaquePointer?
    private var displayLink: CVDisplayLink?
    private var emulationQueue = DispatchQueue(label: "be.pghsoftware.mac68k.emulation",
                                                qos: .userInteractive)

    init() {
        emulator = mac68k_create()
    }

    deinit {
        stopDisplayLink()
        if let emu = emulator {
            mac68k_destroy(emu)
        }
    }

    // MARK: - ROM / Disk

    func openROM() {
        let panel = NSOpenPanel()
        panel.title = "Select Macintosh II ROM"
        panel.allowedContentTypes = [.data]
        panel.allowsMultipleSelection = false

        if panel.runModal() == .OK, let url = panel.url {
            loadROM(url: url)
        }
    }

    func loadROM(url: URL) {
        guard let emu = emulator else { return }
        let path = url.path(percentEncoded: false)
        if mac68k_load_rom(emu, path) {
            statusMessage = "ROM loaded: \(url.lastPathComponent)"
        } else {
            statusMessage = "Failed to load ROM"
        }
    }

    func insertDisk(drive: Int) {
        let panel = NSOpenPanel()
        panel.title = "Insert Floppy Disk"
        panel.allowedContentTypes = [.data]

        if panel.runModal() == .OK, let url = panel.url {
            guard let emu = emulator else { return }
            let path = url.path(percentEncoded: false)
            if mac68k_insert_disk(emu, path, Int32(drive)) {
                statusMessage = "Disk inserted: \(url.lastPathComponent)"
            }
        }
    }

    func mountSCSIDisk() {
        let panel = NSOpenPanel()
        panel.title = "Mount SCSI Hard Disk Image"
        panel.allowedContentTypes = [.data]

        if panel.runModal() == .OK, let url = panel.url {
            // TODO: Select SCSI ID and mount
            statusMessage = "SCSI disk: \(url.lastPathComponent)"
        }
    }

    // MARK: - Emulation control

    func reset() {
        guard let emu = emulator else { return }
        emulationQueue.async {
            mac68k_reset(emu)
        }
        isRunning = true
        statusMessage = "Running"
        startDisplayLink()
    }

    func togglePause() {
        guard let emu = emulator else { return }
        if isRunning {
            mac68k_pause(emu)
            stopDisplayLink()
            statusMessage = "Paused"
        } else {
            mac68k_resume(emu)
            startDisplayLink()
            statusMessage = "Running"
        }
        isRunning.toggle()
    }

    // MARK: - Frame rendering (called by display link)

    func runOneFrame() {
        guard let emu = emulator, isRunning else { return }
        emulationQueue.async {
            mac68k_run_frame(emu)
        }
    }

    /// Get current framebuffer as raw pixel data for Metal texture
    func getFramebuffer() -> UnsafePointer<UInt32>? {
        guard let emu = emulator else { return nil }
        return mac68k_get_framebuffer(emu)
    }

    // MARK: - Input

    func mouseMove(dx: Int, dy: Int) {
        guard let emu = emulator else { return }
        mac68k_mouse_move(emu, Int32(dx), Int32(dy))
    }

    func mouseButton(down: Bool) {
        guard let emu = emulator else { return }
        mac68k_mouse_button(emu, down)
    }

    func keyEvent(keycode: UInt8, down: Bool) {
        guard let emu = emulator else { return }
        mac68k_key_event(emu, keycode, down)
    }

    // MARK: - Debugger

    func updateRegisters() {
        guard let emu = emulator else { return }
        var regs = Mac68kRegisters()
        mac68k_get_registers(emu, &regs)
        self.registers = regs
    }

    func stepInstruction() {
        guard let emu = emulator else { return }
        mac68k_step(emu)
        updateRegisters()
    }

    // MARK: - Display link

    private func startDisplayLink() {
        // TODO: Set up CVDisplayLink to call runOneFrame at 60 Hz
    }

    private func stopDisplayLink() {
        // TODO: Stop CVDisplayLink
    }
}
