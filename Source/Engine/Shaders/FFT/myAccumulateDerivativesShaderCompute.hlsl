#include "FFTComputeCommon.hlsli"

RWTexture2D<float4> Derivatives : register(u0);
Texture2D<float4> PreviousDerivatives : register(t53);


[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float blendFactor = 0.1f;
    
    float4 currentDerivatives = Derivatives[id.xy];
    float4 previousDerivatives = PreviousDerivatives[id.xy];
    
    float4 blendedDerivatives = lerp(previousDerivatives, currentDerivatives, blendFactor);
    
    Derivatives[id.xy] = blendedDerivatives;
}