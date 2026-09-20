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
    float4 posLight : TEXCOORD2;
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
    
    vout.posLight = mul(float4(vin.posL, 1.f), mul(matWorld, g_matShadowLightViewProj));
    return vout;
}

PS_OUT PSMain(VS_OUT pin)
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
    
// 1. 복셀 라이트 계산
    uint blockLightRaw = pin.light & 0xF;
    uint skyLightRaw = (pin.light >> 4) & 0xF;

    float blockLight = blockLightRaw / 15.0f;
    float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;
    float voxelLight = max(skyLight, blockLight);
    
    voxelLight = max(voxelLight, 0.15f);
    
    // 2. 섀도우 맵 판별 (fShadow 계산)
    float fShadow = 0.f; // 0.f: 그림자 속, 1.f: 빛 받음
    {
        float3 vProj = pin.posLight.xyz / pin.posLight.w;
        float2 vShadowUV;
        vShadowUV.x = vProj.x * 0.5f + 0.5f;
        vShadowUV.y = -vProj.y * 0.5f + 0.5f;

        if (saturate(vShadowUV.x) == vShadowUV.x && saturate(vShadowUV.y) == vShadowUV.y)
        {
            float fCurrentDepth = vProj.z - 0.001f; // Acne 방지용 Bias
            fShadow = gShadowMap.SampleCmpLevelZero(gShadowSampler, vShadowUV, fCurrentDepth);
        }
        else
        {
            fShadow = 1.f; // 범위 밖은 빛 받음
        }
    }
    
    // 3. 기본 최종 색상 계산
    float3 finalColor = albedo.rgb * voxelLight;
    
    // ----------------------------------------------------
    // 4. 그림자 감쇄(Shadow Attenuation) 고급 로직
    // ----------------------------------------------------
    // dayFactor가 0.64(해질녘) 이하로 떨어지면 그림자를 서서히 없앰
    float shadowIntensity = saturate((g_fDayFactor - 0.64f) / (1.0f - 0.64f));
    
    // 그림자의 최대 어두움 정도 (기존 하드코딩하셨던 0.3f 유지)
    float targetMaxDarkness = 0.3f;
    
    // 태양의 고도(dayFactor)에 따른 현재 그림자의 기본 밝기 계산
    float minShadowAmbient = lerp(1.0f, targetMaxDarkness, shadowIntensity);
    
    // 횃불(blockLight) 빛과 태양 빛(fShadow) 중 밝은 쪽을 기준으로 그림자를 지워줌
    float fFinalShadow = lerp(minShadowAmbient, 1.f, max(fShadow, blockLight));

    // 완전한 지하/실내(하늘빛이 아예 닿지 않는 곳)에서는 태양 그림자 생성 안 함
    if (skyLightRaw == 0)
    {
        fFinalShadow = 1.0f;
    }
    
    // 5. 최종 색상에 그림자 감쇄 적용
    finalColor *= fFinalShadow;
    
    // 6. 결과 출력
    PS_OUT pout;
    pout.target0 = float4(finalColor, albedo.a);
    return pout;
}