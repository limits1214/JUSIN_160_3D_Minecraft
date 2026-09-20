#include "../ShaderDefines.hlsl"





struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    //float4 boneWeight : BLENDWEIGHT;
    uint boneIndex : BLENDINDICES0;
    uint texIndex : BLENDINDICES1;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 NormalW : TEXCOORD1;
    uint texIndex : BLENDINDICES0;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 NormalW : TEXCOORD1;
    uint texIndex : BLENDINDICES0;
};

struct PS_OUT
{
    float4 target0 : SV_Target;
};

Texture2DArray gEntityTexture64_32Array : register(t7);
Texture2DArray gEntityTexture64_64Array : register(t8);
Texture2DArray gTexture256_256Array : register(t12);
SamplerState gSamPointWrap : register(s0);


VS_OUT VSMain(VS_IN vin)
{
    VS_OUT vout;
    // 1. bone 행렬로 로컬 → 월드 변환
    float4 posW = mul(float4(vin.posL, 1.f), gBoneMatrices[vin.boneIndex]);

    // 2. WVP 곱함
    vout.posH = mul(posW, g_matWVP);
    vout.texCoord = vin.texCoord;
    vout.NormalW = mul(vin.normal, (float3x3) gBoneMatrices[vin.boneIndex]);
    vout.NormalW = mul(vout.NormalW, (float3x3) g_matWorld);
    vout.texIndex = vin.texIndex;
    return vout;
}

// Pixel Shader
PS_OUT PSMain(PS_IN pin)
{
    // TODO: VS에서 넘기기
    uint groupId = GetTexArrayGroup(pin.texIndex);
    uint sliceIndex = GetTexSliceIndex(pin.texIndex);
    float4 albedo;
    
    if(groupId == 7)
    {
        albedo = gEntityTexture64_32Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 8)
    {
        albedo = gEntityTexture64_64Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 12)
    {
        albedo = gTexture256_256Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    
    clip(albedo.a - 0.5f);
    
    /*
    float3 N = normalize(pin.NormalW);
    float3 L = normalize(-gDirLights.direction);
    float NdotL = max(dot(N, L), 0.0f);
    float3 ambient = 0.2f * albedo.rgb;
    float3 diffuse = NdotL * gDirLights.diffuse.rgb * albedo.rgb;

    float3 finalColor = ambient + diffuse;
    */
    
    uint blockLightRaw = g_objectLight & 0xF;
    uint skyLightRaw = (g_objectLight >> 4) & 0xF;

    float blockLight = blockLightRaw / 15.0f;
    float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;
    float voxelLight = max(skyLight, blockLight);
    voxelLight = max(voxelLight, 0.15f);
    
    float3 finalColor = albedo.rgb * voxelLight * g_objectColor.rgb;
    
    PS_OUT pout;
    //pout.target0 = albedo;
    pout.target0 = float4(finalColor, albedo.a);
    return pout;
}