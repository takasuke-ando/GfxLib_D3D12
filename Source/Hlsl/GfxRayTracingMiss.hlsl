





#include "GfxRaytracing.hlsli"
#include "GfxLighting.hlsli"








//  Local Root Signature [Miss]
TextureCube       l_texSky    :   register(t16);






[shader("miss")]
void MyMissShader(inout RayPayload payload)
{

    float3 rayDir = WorldRayDirection();
    rayDir = normalize(rayDir);

    float2 xz = rayDir.xz;

    float theta = atan2(xz.y, xz.x);
    float phai = asin(rayDir.y);


    float2 texcoord;

    texcoord.x = theta / (2 * _PI);
    texcoord.y = -phai / (_PI)+0.5f;


    //float3 skyColor = l_texSky.SampleLevel(sampsLinear, texcoord, 0).xyz;
    float3 skyColor = l_texSky.SampleLevel(sampsLinear, rayDir, 0).xyz;

    payload.color = float3(skyColor);
}


[shader("miss")]
void MyMissShader_Shadow(inout ShadowPayload payload)
{
    payload.isHit = false;
}



