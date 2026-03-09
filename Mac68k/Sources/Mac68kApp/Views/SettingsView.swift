// SettingsView.swift — Emulator preferences

import SwiftUI

struct SettingsView: View {
    @EnvironmentObject var emulator: EmulatorViewModel

    @AppStorage("ramSize") private var ramSize = 8 // MB
    @AppStorage("romPath") private var romPath = ""
    @AppStorage("videoDepth") private var videoDepth = 8

    var body: some View {
        TabView {
            // Hardware tab
            Form {
                Section("Memory") {
                    Picker("RAM Size", selection: $ramSize) {
                        Text("1 MB").tag(1)
                        Text("2 MB").tag(2)
                        Text("4 MB").tag(4)
                        Text("8 MB").tag(8)
                    }
                }

                Section("ROM") {
                    HStack {
                        TextField("ROM Path", text: $romPath)
                        Button("Browse...") {
                            emulator.openROM()
                        }
                    }
                }

                Section("Video") {
                    Picker("Color Depth", selection: $videoDepth) {
                        Text("1-bit (B&W)").tag(1)
                        Text("2-bit (4 colors)").tag(2)
                        Text("4-bit (16 colors)").tag(4)
                        Text("8-bit (256 colors)").tag(8)
                    }
                }
            }
            .tabItem { Label("Hardware", systemImage: "cpu") }
            .padding()
            .frame(width: 400)

            // SCSI tab
            Form {
                Section("SCSI Devices") {
                    ForEach(0..<7, id: \.self) { id in
                        HStack {
                            Text("ID \(id):")
                                .frame(width: 40)
                            Text("—")
                                .foregroundColor(.secondary)
                            Spacer()
                            Button("Mount...") {
                                // TODO: Mount SCSI image to this ID
                            }
                        }
                    }
                }
            }
            .tabItem { Label("SCSI", systemImage: "externaldrive") }
            .padding()
            .frame(width: 400)
        }
    }
}
