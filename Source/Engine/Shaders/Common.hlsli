#define NUMBER_OF_LIGHTS_ALLOWED 256
#define NUMBER_OF_SPOT_LIGHTS_ALLOWED 256
#define MAX_ANIMATION_BONES 128 
#define USE_LIGHTS
#define USE_NOISE
#define PI 3.14159265358979323846
#define COMPUTE_WORK_GROUP_DIM 32
#define WORK_GROUP_DIM_FFT 128
#define DISPLACEMENT_SCALAR 1

#define CLOSE
//#define MID

float2 MultiplyComplex(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y,
                  a.x * b.y + a.y * b.x);
}

float4 ButterflyOperation(float2 a, float2 b, float2 twiddle)
{
    float2 twiddle_b = MultiplyComplex(twiddle, b);
    return float4(a + twiddle_b, a - twiddle_b); // Should use mad
}

int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0;
    int iheight = 0;
    int numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iheight, numMips);
    return numMips;
}

cbuffer CommonBuffer : register(b0)
{
    float4 CameraPosition;
    float4x4 WorldMatrix;
    float4x4 ProjectionMatrix;
    float4x4 ViewMatrix;
};

cbuffer LightConstantBufferData : register(b1)
{
    struct PointLightData
    {
        float4 Position;
        float4 Color;
        float Range;
        float3 garbage;
    } PointLights[NUMBER_OF_LIGHTS_ALLOWED];
    struct SpotLightData
    {
        float4 Position;
        float4 Color;
        float4 Direction;
        float Range;
        float OuterAngle; 
        float InnerAngle;
        float garbage;
    } SpotLights[NUMBER_OF_SPOT_LIGHTS_ALLOWED];
    
    uint NumberOfLights;
    uint NumberOfSpotLights;
    int NumEnvMapMipLevels;
    float garbage0;


    float4 AmbientLightColorAndIntensity;
    
    float4 DirectionalLightDirection;
    
    float3 DirectionalLightColor;
    float DirectionalLightIntensity;
    
};

cbuffer BoneBuffer : register(b3)
{
    float4x4 Bones[MAX_ANIMATION_BONES];
};
cbuffer ShadowDirLightBuffer : register(b4)
{
    float4x4 dirLightViewMatrix;
    float4x4 dirProjectionMatrix;
    float shadowBias;
};

cbuffer TimeBuffer : register(b5)
{
    float3 color;
    float time;
    float2 directionVector;
    float frequency;
    float amplitude;
    float lengthenUV;
    bool isLooping;
};

cbuffer ObjectMatrixBuffer : register(b6)
{
    float4x4 ObjectWorldMatrix;
};

cbuffer BlurBuffer : register(b7)
{
    float2 RenderResolution;
    bool isHorizontal;
    float garbage2;
};

struct Particle
{
    float4 position;
    float4 velocity;
    float4 color;
    float lifetime;
    float size;
    float2 shit;
};



struct PixelOutput
{
    float4 color : SV_TARGET;
};

TextureCube environmentTexture : register(t0);
Texture2D albedoTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D materialTexture : register(t3);
Texture2D shadowMap: register(t4);

Texture2D RT_PositionTexture : register(t5);
Texture2D RT_NormalTexture : register(t6);
Texture2D RT_AlbedoTexture : register(t7);
Texture2D RT_InputNormalTexture : register(t8);
Texture2D RT_MaterialTexture : register(t9);
Texture2D RT_DirectionalLightPositionTexture : register(t10);

Texture2D SceneTexture : register(t12);
Texture2D BloomTexture : register(t13);

Texture2D DepthViewTexture : register(t14);

Texture2D TerrainViewTexture : register(t90);



SamplerState defaultSampler : register(s0);
SamplerState shadowSampler : register(s1);
SamplerState pointSampler : register(s2);
SamplerState waterSampler : register(s3);

struct ModelVertexInput
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



struct ModelVertexToPixel
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float depth : DEPTH;
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
    float4 lightPos : POSITION2;
};


//// This gets Log Depth from worldPosition
//float GetLogDepth(float4 worldPosition)
//{
//    float4 cameraPos = mul(WorldToCamera, worldPosition);
//    float4 projectionPos = mul(CameraToProjection, cameraPos);
//    return projectionPos.z / projectionPos.w;
//}

//float GetLinDepth(float4 worldPosition)
//{
//    float logDepth = GetLogDepth(worldPosition);
//    return NearPlane / (FarPlane - logDepth * (FarPlane - NearPlane));
//}

//// Converts Log Depth to Lin Depth
//float LogDepthToLinDepth(float depth)
//{
//    return NearPlane / (FarPlane - depth * (FarPlane - NearPlane));
//}