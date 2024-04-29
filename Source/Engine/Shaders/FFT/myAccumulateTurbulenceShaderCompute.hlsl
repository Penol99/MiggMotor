#include "FFTComputeCommon.hlsli"

RWTexture2D<float4> Turbulence : register(u0);
Texture2D<float4> PreviousTurbulence : register(t53);


[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float blendFactor = 0.1f;
    float4 currentTurbulence = Turbulence[id.xy];
    float4 previousTurbulence = PreviousTurbulence[id.xy];
    
    float4 blendedTurbulence = lerp(previousTurbulence, currentTurbulence, blendFactor);
    
    Turbulence[id.xy] = blendedTurbulence;
}