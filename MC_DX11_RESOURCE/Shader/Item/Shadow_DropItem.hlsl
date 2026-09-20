#include "../ShaderDefines.hlsl"

struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    
    float4 matWorld0 : INSTANCE_WORLD0;
    float4 matWorld1 : INSTANCE_WORLD1;
    float4 matWorld2 : INSTANCE_WORLD2;
    float4 matWorld3 : INSTANCE_WORLD3;
    uint texIndex : INSTANCE_BLENDINDICES1;
    uint light : INSTANCE_BLENDINDICES2;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES1;
    uint light : BLENDINDICES2;
    float3 normalW : TEXCOORD1;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES1;
    uint light : BLENDINDICES2;
    float3 normalW : TEXCOORD1;
};

struct PS_OUT
{
    float4 target0 : SV_Target;
};

Texture2DArray gItemTexture16_16Array : register(t6);
Texture2DArray gEntityTexture64_32Array : register(t7);
Texture2DArray gEntityTexture64_64Array : register(t8);
Texture2DArray gBlockTextureArray : register(t9);

SamplerState gSamPointWrap : register(s0);

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT vout;

    float4x4 matWorld = float4x4(vin.matWorld0, vin.matWorld1, vin.matWorld2, vin.matWorld3);
    float4x4 matWVP = mul(matWorld, g_matViewProj); // VP만 상수버퍼에서 받기

    vout.posH = mul(float4(vin.posL, 1.f), matWVP);
    vout.texIndex = vin.texIndex;
    vout.texCoord = vin.texCoord; // 인스턴싱이면 uvOffset/Scale은 버텍스에 미리 구워두거나 제거
    vout.normalW = mul(vin.normal, (float3x3) matWorld);
    vout.light = vin.light;
    
    return vout;
}

void PSMain(PS_IN pin)
{
    uint groupId = GetTexArrayGroup(pin.texIndex);
    uint sliceIndex = GetTexSliceIndex(pin.texIndex);
    
    float4 albedo;
    
    if (groupId == 7)
    {
        albedo = gEntityTexture64_32Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 8)
    {
        albedo = gEntityTexture64_64Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 6)
    {
        albedo = gItemTexture16_16Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 9)
    {
        albedo = gBlockTextureArray.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }

    clip(albedo.a - 0.5f);
}