#ifndef		__LIGHTING_H__
#define		__LIGHTING_H__



// ï\ñ ëÆê´
struct Material
{

	float3	DiffuseAlbedo;
	float3	SpecularAlbedo;     //  F0
    float3  Normal;

	float	Roughness;

};

#define _PI     (3.141592f)

struct	DirectionalLight
{
	float3	Dir;		//	To Light
	float3	Irradiance;	//	IrradianceColor
};


float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (_PI * f * f);
}

float3 F_Schlick(float u, float3 f0) {
    return f0 + (float3(1.0f,1.f,1.f) - f0) * pow(1.0 - u, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}

float Fd_Lambert() {
    return 1.0 / _PI;
}



float3		ComputeDirectionalLight(  in Material mat , in DirectionalLight light , float3 Eye /*Pixel to Eye*/ )
{

    float3 v = Eye;

    float3 l = light.Dir;
    float3 n = mat.Normal;

	float3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    // 
    float a = mat.Roughness * mat.Roughness;

    float3 f0 = mat.SpecularAlbedo;


    float D = D_GGX(NoH, a);
    float3  F = F_Schlick(LoH, f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, a);

    // specular BRDF
    float3 Fr = (D * V) * F;

    // diffuse BRDF
    float3 Fd = mat.DiffuseAlbedo * Fd_Lambert();



    return NoL * light.Irradiance * (Fr + Fd);


}





#endif		//__LIGHTING_H__