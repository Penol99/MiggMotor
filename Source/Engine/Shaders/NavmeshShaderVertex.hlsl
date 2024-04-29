cbuffer cBuffer : register(b0)
{
    float4x4 mat;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float4 vertexColor0 : COLOR0;
    float4 vertexColor1 : COLOR1;
    float4 vertexColor2 : COLOR2;
    float4 vertexColor3 : COLOR3;
    float2 texCoord0 : TEXCOORD0;
    float2 texCoord1 : TEXCOORD1;
    float2 texCoord2 : TEXCOORD2;
    float2 texCoord3 : TEXCOORD3;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 boneIndices : BONES;
    float4 weights : WEIGHTS;
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
    float4 position = float4(input.position);

    // Apply the transformation matrix
    output.outPosition = mul(position, mat);
    output.outTexCoord = input.position;
    
    return output;
}