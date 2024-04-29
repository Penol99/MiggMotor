#include "../Common.hlsli"
struct GeometryInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
    float size : SIZE;
};

struct GeometryOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
};

[maxvertexcount(4)]
void main(point GeometryInput input[1], inout TriangleStream<GeometryOutput> outputStream)
{
    float halfSize = input[0].size * 0.5f;

    float4 positions[4] =
    {
        mul(ViewMatrix, float4(input[0].position.xyz + float3(-halfSize, -halfSize, 0), 1)),
        mul(ViewMatrix, float4(input[0].position.xyz + float3(halfSize, -halfSize, 0), 1)),
        mul(ViewMatrix, float4(input[0].position.xyz + float3(-halfSize, halfSize, 0), 1)),
        mul(ViewMatrix, float4(input[0].position.xyz + float3(halfSize, halfSize, 0), 1))
    };

    float2 texCoords[4] =
    {
        float2(0, 1),
        float2(1, 1),
        float2(0, 0),
        float2(1, 0)
    };

    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.position = mul(ProjectionMatrix, positions[i]);
        output.color = input[0].color;
        output.texCoord = texCoords[i];
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}