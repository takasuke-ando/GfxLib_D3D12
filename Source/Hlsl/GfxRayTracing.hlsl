
#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

/***************************************************************
    @file   GfxRayTracing.hlsl
    @par	[説明]

        GfxLibレイトレーシングシェーダファイル

        RayGen及び、Missのみ含む


***************************************************************/




#include "GfxRaytracing.hlsli"
#include "GfxLighting.hlsli"


//  Local Root Signature [RayGen]
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);







//  Local Root Signature [Miss]
TextureCube       l_texSky    :   register(t16);



bool IsInsideViewport(float2 p, Viewport viewport)
{
    return (p.x >= viewport.left && p.x <= viewport.right)
        && (p.y >= viewport.top && p.y <= viewport.bottom);
}

[shader("raygeneration")]
void MyRaygenShader()
{
    float2 lerpValues = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();

    // Orthographic projection since we're raytracing in screen space.
    /*
    float3 rayDir = float3(0, 0, 1);
    float3 origin = float3(
        lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
        lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
        0.0f);
       /*/
    float3 rayDir = float3(
        lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
        lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
        1.0f);
    float3 origin = float3(0, 0, 0);
    //*/

    rayDir = normalize(rayDir);

    rayDir = mul(float4(rayDir, 0.f), g_rayGenCB.mtxCamera).xyz;
    origin = mul( float4(origin,1.f), g_rayGenCB.mtxCamera).xyz;

    //if (IsInsideViewport(origin.xy, g_rayGenCB.stencil))
    {
        /*
            https://microsoft.github.io/DirectX-Specs/d3d/Raytracing.html#addressing-calculations-within-shader-tables
        */
        // Trace the ray.
        // Set the ray's extents.
        RayDesc ray;
        ray.Origin = origin;
        ray.Direction = rayDir;
        // Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
        // TMin should be kept small to prevent missing geometry at close contact areas.
        ray.TMin = 0.001;
        ray.TMax = 10000.0;
        RayPayload payload = { float4(0, 0, 0, 0) };
        //TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
        TraceRay(Scene, 
            RAY_FLAG_NONE,
            ~0,     //  Instance Masks
            0,      //  RayContributionToHitGroupIndex                  :   
            TRACE_TYPE_NUM,      //  MultiplierForGeometryContributionToHitGroupIndex :  BLAS内Geometryのインデックスに、この値を掛けた結果がHitGroupのインデックスとなる
            0,      //  MissShaderIndex
            ray, 
            payload);


        float3 color = payload.color.rgb;

        // Tone Maping
        color = color / (color+1.f);

        // OETF
        color = pow(color, 1 / 2.2f);


        // Write the raytraced color to the output texture.
        RenderTarget[DispatchRaysIndex().xy] = float4(color,1);
    }// else
    //{
        // Render interpolated DispatchRaysIndex outside the stencil window
    //    RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
    //}
}



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
    texcoord.y = - phai / (_PI) + 0.5f;


    //float3 skyColor = l_texSky.SampleLevel(sampsLinear, texcoord, 0).xyz;
    float3 skyColor = l_texSky.SampleLevel(sampsLinear, rayDir, 0).xyz;

    payload.color = float4(skyColor, 1);
}


[shader("miss")]
void MyMissShader_Shadow(inout ShadowPayload payload)
{
    payload.isHit = false;
}



#endif // RAYTRACING_HLSL