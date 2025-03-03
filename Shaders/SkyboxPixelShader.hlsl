TextureCube cube0 : register(t0);
sampler samplerState : register(s0);

struct PIn
{
    float4 position : SV_Position;
    float4 colour : COLOR0;
    float3 uvw : TEXCOORD0;
};

float4 main(PIn input) : SV_TARGET
{
    float4 sampled = cube0.Sample(samplerState, input.uvw);
    return input.colour * sampled;
}