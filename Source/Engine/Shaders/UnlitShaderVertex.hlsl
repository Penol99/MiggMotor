cbuffer cBuffer : register(b0)
{
    float4x4 mat;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL0;
    float2 inTexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // Create a position vector from the input position
    float4 position = float4(input.inPos, 1);

    // Apply the transformation matrix
    output.outPosition = mul(position, mat);
    output.outTexCoord = input.inTexCoord;
    
    return output;
}