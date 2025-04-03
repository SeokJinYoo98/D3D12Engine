
#include "LightUtil.hlsli"

cbuffer cbPerMesh : register(b0)
{
    float4x4 gmtxWorld;
    float4x4 gmtxWorldInv;
    float4   gBaseColor;
};
cbuffer cbMaterial : register(b1)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float  gRoughness;
}
cbuffer cbPass : register(b2)
{
    float4x4                gmtxView;
    float4x4                gmtxProjection;
    float4                  gAmbientLight;
    float3                  gEyePosW;
    int                     gNumDirectionalLights;
    DirectionalLight        gDirectionalLight[MAX_DIRECTION];
    PointLight              gPointLights[MAX_POINT];
    int                     gNumPointsLights;
    float3                  gPadding;
};

// L: Local
// W: World
// H: Homogeneous 클립 공간에서의 정점 위치

struct VS_INPUT
{
    float3 PosL     : POSITION;
    float3 NormL    : NORMAL;
    float2 UV       : UV;
};

struct VS_OUTPUT
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormW    : NORMAL;
};

VS_OUTPUT VSDefault(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 posW = mul(float4(input.PosL, 1.0f), gmtxWorld);
    output.PosW = posW.xyz;

    output.NormW = mul(input.NormL, (float3x3) gmtxWorldInv);
    
    output.PosH = mul(mul(posW, gmtxView), gmtxProjection);
    
    return output;
}

float4 PSDefault(VS_OUTPUT input) : SV_TARGET
{
    input.NormW = normalize(input.NormW);

    float3 toEyeW = normalize(gEyePosW - input.PosW);

    float4 ambient = gAmbientLight * gDiffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    
    float3 result = { 0.0f, 0.0f, 0.0f };
    for (int dir = 0; dir < gNumDirectionalLights; ++dir)
    {
        result += ComputeDirectionalLight
        (
            gDirectionalLight[dir],
            mat,
            input.NormW,
            toEyeW
        );
    }

    for (int points = 0; points < gNumPointsLights; ++points)
    {
        result += ComputePointLight
        (
            gPointLights[points],
            mat,
            input.PosW,
            input.NormW,
            toEyeW
        );
    }
    float4 litColor = ambient + float4(result, 0.f);
    
    litColor.a = gDiffuseAlbedo.a;

    return litColor;
}