cbuffer cbCameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
}
cbuffer cbGameObjectInfo : register(b1)
{
    matrix gmtxWorld : packoffset(c0);
}

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(mul(float4(input.position, 1.0f), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}