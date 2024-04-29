#include "Common.hlsli"

const float4x4 identityMatrix =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};
struct VertexInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float4x4 viewMatrixNoTranslation = ViewMatrix;
    viewMatrixNoTranslation._14 = 0.0;
    viewMatrixNoTranslation._24 = 0.0;
    viewMatrixNoTranslation._34 = 0.0;
    viewMatrixNoTranslation._44 = 1.0;
    output.Position = mul(ProjectionMatrix, mul(viewMatrixNoTranslation, float4(input.Position, 1)));
    output.TexCoord = input.Position;
    
    return output;
}