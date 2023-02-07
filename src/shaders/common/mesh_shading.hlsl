#pragma once

#include "constants.hlsl"

float Pow4(float x)
{
    return (x * x * x * x);
}
float Pow5(float x)
{
    return (x * x * x * x * x);
}

struct BxDFContext
{
    half NoV;
    half NoL;
    half VoL;
    half NoH;
    half VoH;
    half XoV;
    half XoL;
    half XoH;
    half YoV;
    half YoL;
    half YoH;
    
    void Init(half3 N, half3 V, half3 L)
    {
        NoL = dot(N, L);
        NoV = dot(N, V);
        VoL = dot(V, L);
        float InvLenH = rsqrt(2 + 2 * VoL);
        NoH = saturate((NoL + NoV) * InvLenH);
        VoH = saturate(InvLenH + InvLenH * VoL);
		//NoL = saturate( NoL );
		//NoV = saturate( abs( NoV ) + 1e-5 );

        XoV = 0.0f;
        XoL = 0.0f;
        XoH = 0.0f;
        YoV = 0.0f;
        YoL = 0.0f;
        YoH = 0.0f;
    }

    void Init(half3 N, half3 X, half3 Y, half3 V, half3 L)
    {
        NoL = dot(N, L);
        NoV = dot(N, V);
        VoL = dot(V, L);
        float InvLenH = rsqrt(2 + 2 * VoL);
        NoH = saturate((NoL + NoV) * InvLenH);
        VoH = saturate(InvLenH + InvLenH * VoL);
		//NoL = saturate( NoL );
		//NoV = saturate( abs( NoV ) + 1e-5 );

        XoV = dot(X, V);
        XoL = dot(X, L);
        XoH = (XoL + XoV) * InvLenH;
        YoV = dot(Y, V);
        YoL = dot(Y, L);
        YoH = (YoL + YoV) * InvLenH;
    }
};


half3 Diffuse_Lambert(half3 DiffuseColor)
{
    return DiffuseColor * (1 / PI);
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX(float a2, float NoH)
{
    float d = (NoH * a2 - NoH) * NoH + 1; // 2 mad
    return a2 / (PI * d * d); // 4 mul, 1 rcp
}


// Smith term for GGX
// [Smith 1967, "Geometrical shadowing of a random rough surface"]
float Vis_Smith(float a2, float NoV, float NoL)
{
    float Vis_SmithV = NoV + sqrt(NoV * (NoV - NoV * a2) + a2);
    float Vis_SmithL = NoL + sqrt(NoL * (NoL - NoL * a2) + a2);
    return rcp(Vis_SmithV * Vis_SmithL);
}

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox(float a2, float NoV, float NoL)
{
    float a = sqrt(a2);
    float Vis_SmithV = NoL * (NoV * (1 - a) + a);
    float Vis_SmithL = NoV * (NoL * (1 - a) + a);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJoint(float a2, float NoV, float NoL)
{
    float Vis_SmithV = NoL * sqrt(NoV * (NoV - NoV * a2) + a2);
    float Vis_SmithL = NoV * sqrt(NoL * (NoL - NoL * a2) + a2);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}


// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float3 F_Schlick(float3 SpecularColor, float VoH)
{
    float Fc = Pow5(1 - VoH); // 1 sub, 3 mul
	//return Fc + (1 - Fc) * SpecularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
    return saturate(50.0 * SpecularColor.g) * Fc + (1 - Fc) * SpecularColor;
}


float3 BRDF(float3 N, float3 V, float3 L, float3 SpecularColor, float3 BaseColor, float Metallic, float Roughness)
{
    BxDFContext Context;
    
    Context.Init(N, V, L);
    
    Context.NoV = saturate(abs(Context.NoV) + 1e-5);

    
    float a2 = Pow4(Roughness);
	
	// Generalized microfacet specular
    float D = D_GGX(a2, Context.NoH);
    float Vis = Vis_SmithJointApprox(a2, Context.NoV, Context.NoL);
    float3 F = F_Schlick(SpecularColor, Context.VoH);
    
    float3 specular = (D * Vis) * F / (4.0 * Context.NoL * Context.NoV + 0.001);
    
    float3 diffuse = Diffuse_Lambert(BaseColor);
   
    float3 kD = (1.0 - F) * (1.0 - Metallic);
    float3 kS = 1.0 - kD;
    
    return kD * diffuse + kS * specular;
}

