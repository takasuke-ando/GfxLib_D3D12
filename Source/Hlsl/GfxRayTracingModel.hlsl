




/***************************************************************
    @file   GfxRayTracingModel.hlsl
    @par	[説明]

        GfxLibレイトレーシングシェーダファイル

        モデル描画のためのシェーダを含む


***************************************************************/







#include "GfxCommon.hlsli"
#include "GfxRaytracing.hlsli"
#include "GfxLighting.hlsli"




//  l_geomIndexBuffer[MaterialID]
//  今のところ配列の意味なし…
//  モデル内マテリアル毎にHitGroupを分けるのではなく同一にして
//  GeometryIndex()で処理を分けるのであればできるがあまり筋は良くない気がする

ConstantBuffer<ModelConstantBuffer> g_modelCB : register(b16);
ByteAddressBuffer           l_geomIndexBuffer[]     :   register(t16, space1);
StructuredBuffer<VtxAttrib> l_geomAttrib[]          :   register(t16, space2);


Texture2D       l_texDiffuse    :   register(t17);  //  DiffuseMap


typedef BuiltInTriangleIntersectionAttributes MyAttributes;




uint3   GetIndices(uint materialID, uint primitiveID)
{

    primitiveID = g_modelCB.primitiveOffset + primitiveID;

    uint3 indices;
    if (g_modelCB.isIndex16bit) {

        //  16bit

        uint indexOffset = primitiveID * 2 * 3;
        uint dwordIndex = indexOffset & ~0x3;
        uint2 packedIndices = l_geomIndexBuffer[materialID].Load2(dwordIndex);

        if (dwordIndex == indexOffset) {

            indices.x = (packedIndices.x) & 0xffff;
            indices.y = (packedIndices.x >> 16) & 0xffff;
            indices.z = (packedIndices.y) & 0xffff;

        } else {

            indices.x = (packedIndices.x >> 16) & 0xffff;
            indices.y = (packedIndices.y) & 0xffff;
            indices.z = (packedIndices.y >> 16) & 0xffff;

        }
    } else {


        uint indexOffset = primitiveID * 4 * 3;

        indices.xyz = l_geomIndexBuffer[materialID].Load4(indexOffset).xyz;

    }

    return indices;

}





VtxAttrib   GetVtxAttrib(in uint MaterialID, in uint3 Indices, float2 barycentrics)
{

    VtxAttrib attr = (VtxAttrib)0;


    VtxAttrib attr0 = l_geomAttrib[MaterialID].Load(Indices.x);
    VtxAttrib attr1 = l_geomAttrib[MaterialID].Load(Indices.y);
    VtxAttrib attr2 = l_geomAttrib[MaterialID].Load(Indices.z);


    float3  factor = float3(1 - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);

    attr.Normal = attr0.Normal * factor.x + attr1.Normal * factor.y + attr2.Normal * factor.z;
    attr.BaseColor = attr0.BaseColor * factor.x + attr1.BaseColor * factor.y + attr2.BaseColor * factor.z;
    attr.Uv = attr0.Uv * factor.x + attr1.Uv * factor.y + attr2.Uv * factor.z;

    // To World Space
    attr.Normal = mul(attr.Normal, (float3x3)ObjectToWorld4x3());


    return attr;

}



bool        ShadowRayHitTest(float3 worldPosition, float3 lightdir)
{


    RayDesc ray;
    ray.Origin = worldPosition;
    ray.Direction = lightdir;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;
    ShadowPayload payload = { true }; // miss shaderに到達した場合、falseになる
    //TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
    TraceRay(Scene,
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,
        ~0,     //  Instance Masks
        HITGROUPOFFSET_SHADOW,      //  RayContributionToHitGroupIndex                  :   
        TRACE_TYPE_NUM,             //  MultiplierForGeometryContributionToHitGroupIndex :  BLAS内Geometryのインデックスに、この値を掛けた結果がHitGroupのインデックスとなる
        HITGROUPOFFSET_SHADOW,      //  MissShaderIndex
        ray,
        payload);

    return payload.isHit;

}


float2  Hammersley(uint index, uint count , float2 secondRand)
{


    float x = ( index + secondRand .x )/ (float)count;

    float y = 0;

    float base = 0.5f;

    uint val = index;
    while (val) {

        if (val & 1) {

            y += base;

        }


        base *= 0.5f;
        val >>= 1;
    }


    y += secondRand.y;

    if (y > 1) y -= 1;


    return float2(x, y);


}



float3  getCosHemisphereSample(float2 randVal , float3 normal )
{


    float3 bitangent = normalize( cross(float3(0, 0.99f, 0.1f), normal) );
    float3 tangent = cross(bitangent, normal);

    float r = sqrt(randVal.x);

    // ディスク上に一様サンプリング
    float phi = 2.f * _PI * randVal.y;

    //  半球に射影
    float x = r * cos(phi);
    float z = r * sin(phi);
    float y = sqrt(1.f - randVal.x);

    return x * tangent + y * normal + z * bitangent;


}


float   EvaluateAO(float3 position,float3 normal)
{

    uint2   pixelIndex = DispatchRaysIndex().xy;

   // float2 secondRand = frac( pixelIndex * float2(23587.5489,4899.5748) );

    float2 secondRand;

    secondRand.x = Noise((float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions());
    secondRand.y = Noise(((float2)DispatchRaysIndex()+128) / (float2)DispatchRaysDimensions());


    float visibility = 0.f;

    const uint aoRayCount = 32;

    for (uint i = 0; i < aoRayCount; ++i) {


        float2 randVal = Hammersley(i, aoRayCount , secondRand );

        float3 sampleDir = getCosHemisphereSample(randVal,normal);


        bool isHit = ShadowRayHitTest(position,sampleDir);


        float NoL = saturate(dot(normal,sampleDir));

        float pdf = NoL / _PI;

        visibility += (!isHit) * NoL / pdf;


    }


    return (1 / _PI) * visibility / (float)aoRayCount;




}





[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{

    const uint MaterialID = 0;

    const uint  primIndex = PrimitiveIndex();
    const float3 Indices = (float3)GetIndices(MaterialID, primIndex);

    float intepolatedIndices = Indices.x + attr.barycentrics.x * (Indices.y - Indices.x) + attr.barycentrics.y * (Indices.z - Indices.x);

    intepolatedIndices /= 8;

    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);

    VtxAttrib vtx = GetVtxAttrib(MaterialID, Indices, attr.barycentrics);

    //payload.color = float4(barycentrics, 1);
    //payload.color = float4(intepolatedIndices, intepolatedIndices, intepolatedIndices, 1);


    float3  worldPosition = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    const float3 eyeVec = normalize(-WorldRayDirection());  //  ピクセルから視点へ向かうベクトル

    float3 radiance = (float3)0;

    // Directional Light

    Material mat;

    //mat.DiffuseAlbedo = vtx.BaseColor;
    mat.SpecularAlbedo = g_modelCB.specularAlbedo;
    mat.Roughness = g_modelCB.roughness;
    mat.Normal = normalize(vtx.Normal);


    {

        float4 diffuseTex = l_texDiffuse.SampleLevel(sampsLinear, vtx.Uv, 0);

        mat.DiffuseAlbedo = diffuseTex.rgb* vtx.BaseColor * g_modelCB.diffuseAlbedo;

    }

    {
        //  Directional Light
        DirectionalLight   lit;

        //lit.Dir = float3(0.f, 0.707106f, 0.707106f);
        //lit.Dir = float3(0.f, 0.5f, -sqrt(3) / 2.f);
        //lit.Dir = float3(0.f, 1.f, 0.f);
        lit.Dir = normalize(float3(0.f, 0.9f, 0.1f));
        lit.Irradiance = float3(10.f, 10.f, 10.f);

        // Shadowing
        bool isShadow = ShadowRayHitTest(worldPosition, lit.Dir);


        if (!isShadow)
        {
            //radiance += mat.DiffuseAlbedo;
            radiance += ComputeDirectionalLight(mat, lit, eyeVec);
        }
    }

    {
        //  Ambient Light


        float aoOcclusion = EvaluateAO(worldPosition,mat.Normal);

        //  Ambient Diffuse
       radiance += aoOcclusion * g_texSkyIem.SampleLevel(sampsLinear, mat.Normal, 0).xyz * mat.DiffuseAlbedo;


        //  Ambient Specular
        float3 F = F_Schlick(saturate(dot(eyeVec,mat.Normal)), mat.SpecularAlbedo);
       radiance += aoOcclusion * g_texSkyRem.SampleLevel(sampsLinear, reflect(-eyeVec, mat.Normal), mat.Roughness*8.f ).xyz * F;


    }


    payload.color = float4(radiance, 1);
    //payload.color = float4(vtx.BaseColor, 1);

}



// Shadow Closest Hit Shader

[shader("closesthit")]
void MyClosestHitShader_Shadow(inout ShadowPayload payload, in MyAttributes attr)
{
    // 実際のところこちらは呼び出されない (RAY_FLAG_SKIP_CLOSEST_HIT_SHADERを指定しているため)
    payload.isHit = true;
}
