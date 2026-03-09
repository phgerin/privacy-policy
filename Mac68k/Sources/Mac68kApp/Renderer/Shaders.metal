// Shaders.metal — Metal shaders for Mac II screen rendering
//
// Simple fullscreen quad with texture sampling
// The emulator provides an ARGB8888 framebuffer that we display as-is

#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float4 position [[attribute(0)]];
    float2 texcoord [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 texcoord;
};

vertex VertexOut screenVertex(uint vertexID [[vertex_id]],
                               constant float *vertices [[buffer(0)]]) {
    VertexOut out;
    // 6 floats per vertex: x, y, z, w, u, v
    int idx = vertexID * 6;
    out.position = float4(vertices[idx], vertices[idx+1],
                          vertices[idx+2], vertices[idx+3]);
    out.texcoord = float2(vertices[idx+4], vertices[idx+5]);
    return out;
}

fragment float4 screenFragment(VertexOut in [[stage_in]],
                                texture2d<float> screenTex [[texture(0)]]) {
    constexpr sampler texSampler(mag_filter::nearest,
                                 min_filter::nearest);
    return screenTex.sample(texSampler, in.texcoord);
}
