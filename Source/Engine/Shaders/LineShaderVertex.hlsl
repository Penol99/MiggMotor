cbuffer cBuffer : register(b0)
{
    float4x4 mat;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    

    float4 position = float4(input.inPos, 1);

    output.outPosition = mul(position, mat);
    
    return output;
}