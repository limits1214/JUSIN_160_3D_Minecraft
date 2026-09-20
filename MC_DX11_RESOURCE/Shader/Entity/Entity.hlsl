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
    float4 posLight : TEXCOORD2;
};



struct PS_OUT
{
    float4 target0 : SV_Target;
};

Texture2DArray gEntityTexture64_32Array : register(t7);
Texture2DArray gEntityTexture64_64Array : register(t8);
Texture2DArray gTexture256_256Array : register(t12);
Texture2DArray gTexture32_32Array : register(t5);
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
    
    vout.posLight = mul(mul(posW, g_matWorld), g_matShadowLightViewProj);
    return vout;
}

// Pixel Shader
PS_OUT PSMain(VS_OUT pin)
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
    else if(groupId == 5)
    {
        albedo = gTexture32_32Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
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
    

    
    // 1. 복셀 라이트 및 기본 색상 계산
    uint blockLightRaw = g_objectLight & 0xF;
    uint skyLightRaw = (g_objectLight >> 4) & 0xF;

    float blockLight = blockLightRaw / 15.0f;
    float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;
    float voxelLight = max(skyLight, blockLight);
    voxelLight = max(voxelLight, 0.15f);
    
    float3 finalColor = albedo.rgb * voxelLight * g_objectColor.rgb;
    
    // 2. 섀도우 맵 판별 (fShadow 계산)
    float fShadow = 0.f; // 0.f: 그림자, 1.f: 빛 받음
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
    
    // 3. 그림자 감쇄 로직 적용
    // dayFactor 0.64에서 섀도우맵을 안 만듦
    // 자연스럽게 줄이기 위해 1.0 ~ 0.64 까지의 강도를 만듦
    float shadowIntensity = saturate((g_fDayFactor - 0.64f) / (1.0f - 0.64f));
    
    // 최대 강도일 때 그림자 감쇄량
    float targetMaxDarkness = 0.5f;
    
    // 그림자 강도가 0이면 즉, dayFactor가 0.64 아래면 섀도우는 없앤다 (1.0)
    // 그림자 강도가 > 0 이면 targetMaxDarkness 까지 보간함
    float minShadowAmbient = lerp(1.0f, targetMaxDarkness, shadowIntensity);
    
    // 블록 라이팅(횃불 등) 여부에 따라 그림자 감쇄 지움
    // fShadow(태양빛을 받음)와 blockLight(횃불 빛을 받음) 중 더 큰 값을 기준으로 보간
    float fFinalShadow = lerp(minShadowAmbient, 1.f, max(fShadow, blockLight));

    // 스카이 라이트가 0인 경우 (완전한 지하나 지붕 아래) 그림자 감쇄량 무조건 지움
    if (skyLightRaw == 0)
    {
        fFinalShadow = 1.0f;
    }
    
    // 4. 최종 색상에 그림자 감쇄량 곱하기
    finalColor *= fFinalShadow;

    // 5. 출력
    PS_OUT pout;
    pout.target0 = float4(finalColor, albedo.a);
    return pout;
}