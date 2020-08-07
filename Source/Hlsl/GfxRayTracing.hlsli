#ifndef INCLUDE_RAYTRACING_H
#define INCLUDE_RAYTRACING_H






struct Viewport
{
    float left;
    float top;
    float right;
    float bottom;
};

struct RayGenConstantBuffer
{
    Viewport viewport;
    Viewport stencil;
    float4x4 mtxCamera; //  カメラ行列
};


struct ModelConstantBuffer
{
	bool isIndex16bit;
	uint primitiveOffset;   //  モデル全体でのプリミティブ開始位置
	int padd2[2];
};

struct VtxAttrib
{
    float3  Normal;
    float3  BaseColor;
    float2  Uv;
};


struct RayPayload
{
    float4 color;
};

struct ShadowPayload
{
    bool isHit;
};



// --------------------------------------------------------------
// Global Root Signature
// --------------------------------------------------------------

RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);

SamplerState sampsLinear : register(s0);

TextureCube  g_texSky       :   register(t1);
TextureCube  g_texSkyRem    :   register(t2);
TextureCube  g_texSkyIem    :   register(t3);





#define     HITGROUPOFFSET_RADIANCE      (0)
#define     HITGROUPOFFSET_SHADOW        (1)

#define     TRACE_TYPE_NUM        (2)





#endif //INCLUDE_RAYTRACING_H

