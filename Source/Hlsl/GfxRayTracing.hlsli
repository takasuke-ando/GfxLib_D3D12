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
    float   globalTime;
    uint    padd0;
    uint    padd1;
    uint    padd2;
};


struct ModelConstantBuffer
{
	bool isIndex16bit;
	uint primitiveOffset;   //  モデル全体でのプリミティブ開始位置
	int2 padd;
    float3  diffuseAlbedo;
    float   roughness;
    float3  specularAlbedo;
    int     padd2;
    float3  emissive;
    int     padd3;
};

struct VtxAttrib
{
    float3  Normal;
    float3  BaseColor;
    float2  Uv;
};


struct RayPayload
{
    float3 color;
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


ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);



//  通常モデル
#define     HITGROUPOFFSET_RADIANCE           (0)
//  低品質描画
#define     HITGROUPOFFSET_RADIANCE_LOW      (1)
#define     HITGROUPOFFSET_SHADOW           (2)


#define     MISSOFFSET_RADIANCE      (0)
#define     MISSOFFSET_SHADOW        (1)


#define     TRACE_TYPE_NUM          (3)

#define     GI_RAY_COUNT            (4)


#define     AMBIENT_DIFFUSE_TYPE_NONE  (0)
#define     AMBIENT_DIFFUSE_TYPE_GI  (1)
#define     AMBIENT_DIFFUSE_TYPE_SKY_LIGHT  (2)


enum class AMBIENT_DIFFUSE_TYPE{

    None,
    GI,
    SkyLight,
};


#define     AMBIENT_SPECULAR_TYPE_NONE       (0)
#define     AMBIENT_SPECULAR_TYPE_GI          (1)
#define     AMBIENT_SPECULAR_TYPE_SKY_LIGHT  (2)


enum class AMBIENT_SPECULAR_TYPE {

    None,
    GI,
    SkyLight,

};


#endif //INCLUDE_RAYTRACING_H

