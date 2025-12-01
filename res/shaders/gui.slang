[[vk::binding(0, 0)]]
struct Textures {
    Sampler2DArray blocks;
    Sampler2DArray gui;
};
ConstantBuffer<Textures> textures;



struct VertexInput {
    float2 pos;
    float texIndex;
};



struct VertexOutput {
    float4 position : SV_Position;
    float4 tex;
};



static float2 QUAD_UV_COORDINATES[] = {
    float2(0.0, 0.0),
    float2(1.0, 0.0),
    float2(1.0, 1.0),
    float2(0.0, 1.0)
};



[shader("vertex")]
VertexOutput vertMain(VertexInput input, uint vertexIndex : SV_VertexID) {
    VertexOutput output;
    output.position = float4(input.pos, 0.0, 1.0);
    output.tex = float4(
        QUAD_UV_COORDINATES[vertexIndex & 3],
        input.texIndex,
        0.0
    );
    return output;
}



[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    float4 texColour = textures.gui.Sample(inVert.tex.xyz);
    if (texColour.a < 0.1) {
        discard;
    }
    return float4(texColour.rgb, 1.0);
}
