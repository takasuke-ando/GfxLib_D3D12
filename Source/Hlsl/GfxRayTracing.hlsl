
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
//ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b16);


RWTexture2D<float4> l_outSceneHDR   : register(u1);
Texture2D<float4>    l_texPrevHDR    : register(t16);






bool IsInsideViewport(float2 p, Viewport viewport)
{
    return (p.x >= viewport.left && p.x <= viewport.right)
        && (p.y >= viewport.top && p.y <= viewport.bottom);
}



/*
    人間の知覚反応に近いカラースペースに変換
*/
float3  LinearToPerceptual(float3 color)
{
    return log2(max(color,(float3)0.0001f));
}


float3  PerceptualToLinear(float3 percep)
{
    return pow(2.f,percep);
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
        RayPayload payload = { float3(0, 0, 0) };
        //TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
        TraceRay(Scene, 
            RAY_FLAG_NONE,
            ~0,     //  Instance Masks
            HITGROUPOFFSET_RADIANCE,      //  RayContributionToHitGroupIndex                  :   
            TRACE_TYPE_NUM,      //  MultiplierForGeometryContributionToHitGroupIndex :  BLAS内Geometryのインデックスに、この値を掛けた結果がHitGroupのインデックスとなる
            0,      //  MissShaderIndex
            ray, 
            payload);


        float3 color = payload.color.rgb;


        uint2 pixelIndex = DispatchRaysIndex().xy;


        float3 prevColor = l_texPrevHDR.Load(uint3(pixelIndex,0)).rgb;

        color = LinearToPerceptual(color);

        float feedback = 0.97f;
        color = prevColor * feedback + color * (1- feedback);

        l_outSceneHDR[pixelIndex] = float4(color,0);

        color = PerceptualToLinear(color);

        // Tone Maping
        color = color / (color+1.f);

        // OETF
        color = pow(color, 1 / 2.2f);

        // Write the raytraced color to the output texture.
        RenderTarget[pixelIndex] = float4(color,1);
    }// else
    //{
        // Render interpolated DispatchRaysIndex outside the stencil window
    //    RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
    //}
}





#endif // RAYTRACING_HLSL