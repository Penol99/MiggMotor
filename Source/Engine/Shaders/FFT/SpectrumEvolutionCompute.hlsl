#include "FFTComputeCommon.hlsli"


RWTexture2D<float2> Dx_Dz   : register(u0);
RWTexture2D<float2> Dy_Dxz  : register(u1);
RWTexture2D<float2> Dyx_Dyz : register(u2);
RWTexture2D<float2> Dxx_Dzz : register(u3);

Texture2D<float4> H0        : register(t53);
Texture2D<float4> WavesData : register(t54);


[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float4 wave = WavesData[id.xy];
    float phase = wave.w * Time;
    float2 exponent = float2(cos(phase), sin(phase));
    float2 h = ComplexMult(H0[id.xy].xy, exponent) + ComplexMult(H0[id.xy].zw, float2(exponent.x, -exponent.y));
    float2 ih = float2(-h.y, h.x);
	
    float2 displacementX = ih * wave.x * wave.y;
    float2 displacementY = h;
    float2 displacementZ = ih * wave.z * wave.y;
		 
    float2 displacementX_dx = -h * wave.x * wave.x * wave.y;
    float2 displacementY_dx = ih * wave.x;
    float2 displacementZ_dx = -h * wave.x * wave.z * wave.y;
		 
    float2 displacementY_dz = ih * wave.z;
    float2 displacementZ_dz = -h * wave.z * wave.z * wave.y;
	
    Dx_Dz[id.xy] = float2(displacementX.x - displacementZ.y, displacementX.y + displacementZ.x);
    Dy_Dxz[id.xy] = float2(displacementY.x - displacementZ_dx.y, displacementY.y + displacementZ_dx.x);
    Dyx_Dyz[id.xy] = float2(displacementY_dx.x - displacementY_dz.y, displacementY_dx.y + displacementY_dz.x);
    Dxx_Dzz[id.xy] = float2(displacementX_dx.x - displacementZ_dz.y, displacementX_dx.y + displacementZ_dz.x);
}