struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float2 outTexCoord : TEXCOORD0; // Texture coordinates
};

VS_OUTPUT main(uint vI : SV_VERTEXID) 
{
    VS_OUTPUT output;
    output.outTexCoord = float2(vI & 1, vI >> 1);
    output.outPos = float4((output.outTexCoord.x - 0.5f) * 2, -(output.outTexCoord.y - 0.5f) * 2, 0, 1);
    return output;
}