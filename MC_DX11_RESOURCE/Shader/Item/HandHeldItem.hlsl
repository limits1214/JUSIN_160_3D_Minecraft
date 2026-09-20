#include "../ShaderDefines.hlsl"

struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES1;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES1;
    float3 normalW : TEXCOORD1;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES1;
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

    vout.posH = mul(float4(vin.posL, 1.f), g_matWVP);
    vout.texIndex = vin.texIndex;
    vout.texCoord = vin.texCoord; 
    vout.normalW = mul(vin.normal, (float3x3) g_matWorld);
    
    return vout;
}

PS_OUT PSMain(PS_IN pin)
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
    
    uint blockLightRaw = g_objectLight & 0xF;
    uint skyLightRaw = (g_objectLight >> 4) & 0xF;

    float blockLight = blockLightRaw / 15.0f;
    float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;
    float voxelLight = max(skyLight, blockLight);
    
    voxelLight = max(voxelLight, 0.15f);

    /*
    float3 N = normalize(pin.normalW);
    float3 L = normalize(-gDirLights.direction);
    float NdotL = max(dot(N, L), 0.0f);
    float3 ambient = 0.2f * albedo.rgb;
    float3 diffuse = NdotL * gDirLights.diffuse.rgb * albedo.rgb;

*/
    
    float3 finalColor = albedo.rgb * voxelLight;

    PS_OUT pout;
    pout.target0 = float4(finalColor, albedo.a);
    return pout;
}