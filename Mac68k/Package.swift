// swift-tools-version: 5.9

import PackageDescription

let package = Package(
    name: "Mac68k",
    platforms: [
        .macOS(.v14)
    ],
    products: [
        .executable(name: "Mac68k", targets: ["Mac68kApp"])
    ],
    targets: [
        // C++ emulator core (Moira + hardware emulation)
        .target(
            name: "Mac68kCore",
            path: "Sources/Mac68kCore",
            sources: [
                "CPU/MoiraBridge.cpp",
                "Memory/MemoryManager.cpp",
                "Hardware/VIA6522.cpp",
                "Hardware/ADB.cpp",
                "Hardware/SCSIManager.cpp",
                "Hardware/ASC.cpp",
                "Hardware/SCC.cpp",
                "Hardware/SWIM.cpp",
                "Hardware/NuBus.cpp",
                "Hardware/VideoCard.cpp"
            ],
            publicHeadersPath: "include",
            cxxSettings: [
                .headerSearchPath("Moira"),
                .headerSearchPath("CPU"),
                .headerSearchPath("Hardware"),
                .headerSearchPath("Memory"),
                .define("MOIRA_PRECISION", to: "2"),
                .define("MOIRA_BUS_ERROR", to: "1")
            ]
        ),
        // Swift app with SwiftUI + Metal
        .executableTarget(
            name: "Mac68kApp",
            dependencies: ["Mac68kCore"],
            path: "Sources/Mac68kApp",
            swiftSettings: [
                .interoperabilityMode(.Cxx)
            ]
        ),
        .testTarget(
            name: "Mac68kTests",
            dependencies: ["Mac68kCore"],
            path: "Tests"
        )
    ],
    cxxLanguageStandard: .cxx20
)
