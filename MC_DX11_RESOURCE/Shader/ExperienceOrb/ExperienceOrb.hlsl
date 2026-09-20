#include "../ShaderDefines.hlsl"

struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    
    // Instanced Data
    float4 matWorld0 : INSTANCE_WORLD0;
    float4 matWorld1 : INSTANCE_WORLD1;
    float4 matWorld2 : INSTANCE_WORLD2;
    float4 matWorld3 : INSTANCE_WORLD3;
    float2 uvOffset : INSTANCE_UVOFFSET; 
    float4 vColor : INSTANCE_COLOR;
    uint light : INSTANCE_LIGHT;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float4 vColor : COLOR0;
    uint light : BLENDINDICES0;
};

Texture2DArray gEntityTexture64_64Array : register(t8);
Texture2D gExpOrbTexture : register(t0);
SamplerState gSamPointWrap : register(s0);

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT vout;

    float4x4 matWorld = float4x4(vin.matWorld0, vin.matWorld1, vin.matWorld2, vin.matWorld3);
    float4x4 matWVP = mul(matWorld, g_matViewProj);

    vout.posH = mul(float4(vin.posL, 1.f), matWVP);

    vout.texCoord = (vin.texCoord * 0.25f) + vin.uvOffset;

    vout.light = vin.light;
    
    vout.vColor = vin.vColor;
    
    return vout;
}

struct PS_OUT
{
    float4 target0 : SV_Target;
};

PS_OUT PSMain(VS_OUT pin)
{
    float4 albedo = gEntityTexture64_64Array.Sample(gSamPointWrap, float3(pin.texCoord, 4.f) );

    clip(albedo.a - 0.5f);
    
    albedo.rgb *= pin.vColor.rgb;
    
    uint blockLightRaw = pin.light & 0xF;
    uint skyLightRaw = (pin.light >> 4) & 0xF;

    float blockLight = blockLightRaw / 15.0f;
    float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;
    float voxelLight = max(skyLight, blockLight);
    
    // 경험치 오브 발광 (최소 조명 확보)
    voxelLight = max(voxelLight, 0.8f);

    float3 finalColor = albedo.rgb * voxelLight;

    PS_OUT pout;
    pout.target0 = float4(finalColor, albedo.a);
    
    return pout;
}