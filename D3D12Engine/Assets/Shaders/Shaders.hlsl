cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
    float4 color : packoffset(c4);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
}

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView),gmtxProjection);
    output.color = color;
    return (output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}