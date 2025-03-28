
struct Light
{
    float4x4 Transform;
    float3 Color;
    float Intensity;
};
struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};
float3 GetPosition(matrix transform)
{
    return transform[3].xyz;
}
float3 GetDirection(matrix transform)
{
    return -normalize(transform[2].xyz);
}