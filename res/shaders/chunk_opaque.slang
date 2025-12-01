struct UniformBuffer {
    float4x4 transform;
};
[[vk::push_constant]] UniformBuffer ubo;



[[vk::binding(0, 0)]]
struct Textures {
    Sampler2DArray blocks;
    Sampler2DArray gui;
};
ConstantBuffer<Textures> textures;



struct VertexInput {
    uint4 posPlusTexPos;
    float texIndex;
    float light;
};



struct VertexOutput {
    float4 position : SV_Position;
    float4 fragTexCoordsPlusLight;
};



[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    output.position = mul(ubo.transform, float4(float3(input.posPlusTexPos.xyz) / 32.0, 1.0));
    output.fragTexCoordsPlusLight = float4(
        input.posPlusTexPos.a & 1,
        (input.posPlusTexPos.a & 2) >> 1,
        input.texIndex,
        input.light
    );
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    float4 texColour = textures.blocks.Sample(inVert.fragTexCoordsPlusLight.xyz);
    return float4(texColour.xyz * inVert.fragTexCoordsPlusLight.w, 1.0);
}
