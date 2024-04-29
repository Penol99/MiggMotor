#include "FFTComputeCommon.hlsli"

RWTexture2D<float3> Displacement : register(u0);
RWTexture2D<float4> Derivatives : register(u1);
RWTexture2D<float4> Turbulence : register(u2);

Texture2D<float2> Dx_Dz : register(t53);
Texture2D<float2> Dy_Dxz : register(t54);
Texture2D<float2> Dyx_Dyz : register(t55);
Texture2D<float2> Dxx_Dzz : register(t56);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float2 coordinates = id.xy;

    float2 dxDz = Dx_Dz[coordinates];
    float2 dyDxz = Dy_Dxz[coordinates];
    float2 dyxDyz = Dyx_Dyz[coordinates];
    float2 dxxDzz = Dxx_Dzz[coordinates];

    float3 displacement;
    displacement.x = Lambda * dxDz.x;
    displacement.y = dyDxz.x;
    displacement.z = Lambda * dxDz.y;

    float4 derivatives;
    derivatives.xy = dyxDyz;
    derivatives.zw = dxxDzz * Lambda;

    float jacobian = (1 + Lambda * dxxDzz.x) * (1 + Lambda * dxxDzz.y) - Lambda * Lambda * dyDxz.y * dyDxz.y;

    float4 turbulence = Turbulence[coordinates];
    turbulence.r += DeltaTime * 0.5 / max(jacobian, 0.5);
    turbulence.r = min(jacobian, turbulence.r);

    Displacement[coordinates] = displacement;
    Derivatives[coordinates] = derivatives;
    Turbulence[coordinates] = turbulence;
}