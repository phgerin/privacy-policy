// ContentView.swift — Main emulator window

import SwiftUI

struct ContentView: View {
    @EnvironmentObject var emulator: EmulatorViewModel

    var body: some View {
        VStack(spacing: 0) {
            // Emulated screen (Metal rendering)
            MetalScreenView()
                .aspectRatio(CGFloat(emulator.screenWidth) / CGFloat(emulator.screenHeight),
                             contentMode: .fit)
                .frame(minWidth: 512, minHeight: 384)

            // Status bar
            HStack {
                Circle()
                    .fill(emulator.isRunning ? .green : .red)
                    .frame(width: 8, height: 8)
                Text(emulator.statusMessage)
                    .font(.caption)
                    .foregroundColor(.secondary)
                Spacer()
            }
            .padding(.horizontal, 8)
            .padding(.vertical, 4)
            .background(.bar)
        }
        .overlay {
            if emulator.showDebugger {
                DebuggerView()
                    .frame(width: 320)
                    .frame(maxHeight: .infinity)
                    .background(.ultraThinMaterial)
                    .transition(.move(edge: .trailing))
            }
        }
    }
}
