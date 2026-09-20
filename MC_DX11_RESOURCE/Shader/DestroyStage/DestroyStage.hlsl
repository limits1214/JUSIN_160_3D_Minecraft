#include "../ShaderDefines.hlsl"

Texture2DArray gCrackArray : register(t11); // destroy_stage 0~9
SamplerState gSampler : register(s0);



struct PS_OUT
{
    float4 target0 : SV_Target;
};
struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0; // 추가
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0; // triplanar 필요없으니 posW 제거
};

VS_OUT VSMain(VS_IN vin)
{
    float3 posL = vin.posL + vin.normal * 0.001f; // 노멀 방향으로 살짝 push

    VS_OUT vout;
    vout.posH = mul(float4(posL, 1.f), g_matWVP);
    vout.texCoord = vin.texCoord;
    return vout;
}

PS_OUT PSMain(VS_OUT pin)
{
    float4 crack = gCrackArray.Sample(gSampler, float3(pin.texCoord, g_destroyStage));
    clip(crack.a - 0.2f);
    
    uint blockLightRaw = g_destroyStageLight & 0xF;
    uint skyLightRaw = (g_destroyStageLight >> 4) & 0xF;

    float blockLight = blockLightRaw / 15.0f;
    float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;

    float finalLight = max(skyLight, blockLight);
    
    float alphaLight = saturate(finalLight);
    
    // 최소 암전 방지 (완전 흑암 속에서도 금 형태는 아주 미세하게 보아하므로 하한선 지정)
    alphaLight = max(alphaLight, 0.20f);

    PS_OUT pout;
    pout.target0 = float4(crack.rgb, crack.a * alphaLight);
    
    return pout;
}