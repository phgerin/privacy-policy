// MetalScreenView.swift — Metal-accelerated emulated screen rendering
//
// Renders the Mac II framebuffer (ARGB8888) as a Metal texture
// displayed in a SwiftUI view via NSViewRepresentable.

import SwiftUI
import MetalKit

struct MetalScreenView: NSViewRepresentable {
    @EnvironmentObject var emulator: EmulatorViewModel

    func makeNSView(context: Context) -> MTKView {
        let mtkView = MTKView()
        mtkView.device = MTLCreateSystemDefaultDevice()
        mtkView.delegate = context.coordinator
        mtkView.enableSetNeedsDisplay = false
        mtkView.isPaused = false
        mtkView.preferredFramesPerSecond = 60
        mtkView.colorPixelFormat = .bgra8Unorm

        // Accept keyboard and mouse events
        mtkView.acceptsTouchEvents = true

        context.coordinator.setup(device: mtkView.device!, size: mtkView.drawableSize)

        return mtkView
    }

    func updateNSView(_ nsView: MTKView, context: Context) {
        context.coordinator.emulator = emulator
    }

    func makeCoordinator() -> ScreenRenderer {
        ScreenRenderer()
    }
}

class ScreenRenderer: NSObject, MTKViewDelegate {
    var emulator: EmulatorViewModel?

    private var device: MTLDevice?
    private var commandQueue: MTLCommandQueue?
    private var pipelineState: MTLRenderPipelineState?
    private var screenTexture: MTLTexture?
    private var vertexBuffer: MTLBuffer?

    func setup(device: MTLDevice, size: CGSize) {
        self.device = device
        self.commandQueue = device.makeCommandQueue()

        // Create fullscreen quad vertices
        let vertices: [Float] = [
            // positions     // texcoords
            -1,  1, 0, 1,   0, 0,  // top-left
             1,  1, 0, 1,   1, 0,  // top-right
            -1, -1, 0, 1,   0, 1,  // bottom-left
             1, -1, 0, 1,   1, 1,  // bottom-right
        ]
        vertexBuffer = device.makeBuffer(bytes: vertices,
                                          length: vertices.count * MemoryLayout<Float>.size,
                                          options: .storageModeShared)

        // Create screen texture (640x480 BGRA8)
        let texDesc = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: .bgra8Unorm,
            width: 640,
            height: 480,
            mipmapped: false
        )
        texDesc.usage = [.shaderRead]
        screenTexture = device.makeTexture(descriptor: texDesc)

        setupPipeline(device: device)
    }

    private func setupPipeline(device: MTLDevice) {
        // Load shaders from the default library
        guard let library = device.makeDefaultLibrary(),
              let vertexFunc = library.makeFunction(name: "screenVertex"),
              let fragmentFunc = library.makeFunction(name: "screenFragment") else {
            // Shaders not compiled yet — will be added when Metal shaders are created
            return
        }

        let pipelineDesc = MTLRenderPipelineDescriptor()
        pipelineDesc.vertexFunction = vertexFunc
        pipelineDesc.fragmentFunction = fragmentFunc
        pipelineDesc.colorAttachments[0].pixelFormat = .bgra8Unorm

        pipelineState = try? device.makeRenderPipelineState(descriptor: pipelineDesc)
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        // Handle window resize
    }

    func draw(in view: MTKView) {
        // Run one emulation frame
        Task { @MainActor in
            emulator?.runOneFrame()
        }

        // Update screen texture from emulator framebuffer
        if let fb = emulator?.getFramebuffer(), let texture = screenTexture {
            let region = MTLRegion(origin: MTLOrigin(x: 0, y: 0, z: 0),
                                   size: MTLSize(width: 640, height: 480, depth: 1))
            texture.replace(region: region,
                           mipmapLevel: 0,
                           withBytes: fb,
                           bytesPerRow: 640 * 4)
        }

        guard let drawable = view.currentDrawable,
              let renderPassDesc = view.currentRenderPassDescriptor,
              let commandBuffer = commandQueue?.makeCommandBuffer(),
              let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDesc) else {
            return
        }

        if let pipeline = pipelineState {
            encoder.setRenderPipelineState(pipeline)
            encoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
            encoder.setFragmentTexture(screenTexture, index: 0)
            encoder.drawPrimitives(type: .triangleStrip, vertexStart: 0, vertexCount: 4)
        }

        encoder.endEncoding()
        commandBuffer.present(drawable)
        commandBuffer.commit()
    }
}
