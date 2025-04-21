cbuffer constantBuffer
{
    matrix wvp;
};

struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR0;
    float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_Position;
    float4 colour : COLOR0;
    float2 uv : TEXCOORD0;
};

VOut main(VIn input)
{
    VOut output;
    
    output.position = mul(wvp, float4(input.position, 1.0f));
    output.colour = input.colour;
    output.uv = input.uv;
    
    return output;
}