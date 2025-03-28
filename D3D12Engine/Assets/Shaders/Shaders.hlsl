#include "LightUtil.hlsl"
#define MAX_LIGHTS 16
cbuffer cbPerMesh : register(b0)
{
    float4x4 gmtxWorld;
    float4 color;
};

cbuffer cbPass : register(b1)
{
    float4x4    gmtxView;
    float4x4    gmtxProjection;
    float3      gEyePosW;
    float       gCbPadding;
    float4      gAmbientLight;
    Light       gLight;
};

// L: Local
// W: World
// H: Homogeneous 클립 공간에서의 정점 위치

struct VS_INPUT
{
    float3 PosL : POSITION;
    float3 NormL : NORMAL;
    float2 UV : UV;
};

struct VS_OUTPUT
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormW    : NORMAL;
    float4 Color    : COLOR;
};

VS_OUTPUT VSDefault(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 posW = mul(float4(input.PosL, 1.0f), gmtxWorld);
    
    output.PosW = posW.xyz;
    output.NormW = mul(input.NormL, (float3x3) gmtxWorld);
    output.PosH = mul(mul(posW, gmtxView), gmtxProjection);
    output.Color = color;
    
    return (output);
}

float4 PSDefault(VS_OUTPUT input) : SV_TARGET
{
    const float shininess = 1.f;
    float3 N = normalize(input.NormW);
    float3 L = normalize(GetPosition(gLight.Transform) - input.PosW);
    float3 V = normalize(gEyePosW - input.PosW);
    
    // 반사 벡터 계산 (Phong 모델의 specular 용)
    float3 R = reflect(-L, N);
    // Ambient: 환경 조명 (전역 조명)
    float3 ambient = gAmbientLight.rgb;
    // Diffuse: Lambertian 반사 (두 벡터 내적 사용)
    float diff = saturate(dot(N, L));
    float3 diffuse = gLight.Color * diff * gLight.Intensity;
    // Specular: 반사된 빛과 시선 벡터 간 내적
    float spec = pow(saturate(dot(V, R)), shininess);
    float3 specular = gLight.Color * spec * gLight.Intensity;
    // 최종 색상: Ambient + Diffuse + Specular
    float3 lighting = ambient + diffuse + specular;
    float3 finalColor = lighting * input.Color.rgb;
    
    return float4(finalColor, 1.f);
}