#include "../ShaderDefines.hlsl"

struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    uint faceId : BLENDINDICES0;

    float4 matWorld0 : INSTANCE_WORLD0;
    float4 matWorld1 : INSTANCE_WORLD1;
    float4 matWorld2 : INSTANCE_WORLD2;
    float4 matWorld3 : INSTANCE_WORLD3;
    uint texIndex0 : INSTANCE_BLENDINDICES1;
    uint texIndex1 : INSTANCE_BLENDINDICES2;
    uint texIndex2 : INSTANCE_BLENDINDICES3;
    uint texIndex3 : INSTANCE_BLENDINDICES4;
    uint texIndex4 : INSTANCE_BLENDINDICES5;
    uint texIndex5 : INSTANCE_BLENDINDICES6;
    uint light : INSTANCE_BLENDINDICES7;
    float4 vColor : INSTANCE_COLOR0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES0;
};

Texture2DArray gBlockTextureArray : register(t9);
SamplerState gSamPointWrap : register(s0);

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT vout;
    float4x4 matWorld = float4x4(vin.matWorld0, vin.matWorld1, vin.matWorld2, vin.matWorld3);
    vout.posH = mul(float4(vin.posL, 1.f), mul(matWorld, g_matViewProj));

    // alpha clip용 texIndex 전달
    uint faceTexIndex;
    if (vin.faceId == 0)
        faceTexIndex = vin.texIndex0;
    else if (vin.faceId == 1)
        faceTexIndex = vin.texIndex1;
    else if (vin.faceId == 2)
        faceTexIndex = vin.texIndex2;
    else if (vin.faceId == 3)
        faceTexIndex = vin.texIndex3;
    else if (vin.faceId == 4)
        faceTexIndex = vin.texIndex4;
    else
        faceTexIndex = vin.texIndex5;
    vout.texIndex = faceTexIndex;
    vout.texCoord = vin.texCoord;
    return vout;
}

void PSMain(VS_OUT pin)  
{
    uint groupId = GetTexArrayGroup(pin.texIndex);
    uint sliceIndex = GetTexSliceIndex(pin.texIndex);

    float4 albedo = gBlockTextureArray.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    clip(albedo.a - 0.5f);
}