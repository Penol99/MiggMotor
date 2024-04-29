struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(0.0, 1.0, 0.0, 1.0); // Set line color with desired alpha value
}