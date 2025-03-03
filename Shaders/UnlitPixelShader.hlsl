Texture2D texture0 : register(t0);
sampler samplerState : register(s0);

struct PIn
{
    float4 position : SV_Position;
    float4 colour : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PIn input) : SV_TARGET
{
    float4 sampled = texture0.Sample(samplerState, input.uv);
    return input.colour * sampled;
}