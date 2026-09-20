#include "../ShaderDefines.hlsl"
// 0000 0000  0000 0000  0000 0000  0000 0000
// 
// normal(3)
// 1110 0000  0000 0000  0000 0000  0000 0000

// vertexao(2)
// 0001 1000  0000 0000  0000 0000  0000 0000

// vertexid(2)
// 0000 0110  0000 0000  0000 0000  0000 0000

// textureid(8)
// 0000 0001  1111 1110  0000 0000  0000 0000

// light(8)
// 0000 0000  0000 0001  1111 1110  0000 0000

struct VS_IN
{
    float3 posL : POSITION;
    float2 texCoord : TEXCOORD0;
    float4 vColor : COLOR_PACK;
    uint packedData : BLENDINDICES;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float4 vColor : COLOR_PACK;
    float3 NormalW : TEXCOORD1;
    uint texID : BLENDINDICES0;
    uint light : BLENDINDICES1;
    float ao : BLENDWEIGHT0;
    float4 posLight : TEXCOORD2;
};


struct PS_OUT
{
    float4 target0 : SV_Target;
};

Texture2DArray gBlockTextureArray : register(t9);
SamplerState gSamPointWrap : register(s9);


Texture2D gLavaStillTexture : register(t15);
Texture2D gLavaFlowTexture : register(t16);

static const float3 gNORMALS[6] =
{
    float3(1, 0, 0), float3(-1, 0, 0), // X
    float3(0, 1, 0), float3(0, -1, 0), // Y
    float3(0, 0, 1), float3(0, 0, -1) // Z
};

static const float2 gUV[4] =
{
    float2(0, 0), float2(1, 0),
    float2(1, 1), float2(0, 1)
};

VS_OUT VSMain(VS_IN vin)
{
    uint normalID = (vin.packedData >> 29) & 0x07;
    float3 worldNormal = gNORMALS[normalID];
    
    uint aoID = (vin.packedData >> 27) & 0x03;
    //float aoFactor = 0.4f + (aoID / 3.0f) * 0.6f;
    float aoValues[4] = { 0.4f, 0.6f, 0.8f, 1.0f }; // 0일 때 가장 어둡고, 3일 때 온전한 밝기
    float aoFactor = aoValues[aoID];
    uint vtxID = (vin.packedData >> 25) & 0x03;
    float2 uv = gUV[vtxID];
    
    uint texID = (vin.packedData >> 17) & 0xff;
    
    float2 vTexCoord;
    vTexCoord = vin.texCoord;
    if (texID == 15)
    {
        float2 size = float2(16.f / 16.f, 16.f / 512.f);
        float u = 0.f;
        float v = (16.f / 512.f) * float(g_stillFrameIndex);
        float2 uvOffset = float2(u, v);
    
        vTexCoord = uvOffset + (vin.texCoord * size);
    }
    else if (texID == 16)
    {
        float2 size = float2(16.f / 16.f, 16.f / 512.f);
        float u = 0.f;
        float v = (16.f / 512.f) * float(g_stillFrameIndex);
        float2 uvOffset = float2(u, v);
    
        vTexCoord = uvOffset + (vin.texCoord * size);
    }
    
    VS_OUT vout;
    vout.posH = mul(float4(vin.posL, 1.f), g_matWVP);
    //output.pos = float4(input.pos, 1.0f);
    //vout.col = vin.col;
    //vout.texCoord = uv;
    vout.texCoord = vTexCoord;
    vout.NormalW = mul(worldNormal, (float3x3) g_matWorld);
    vout.texID = texID;
    vout.light = (vin.packedData >> 9) & 0xff;
    vout.ao = aoFactor;
    vout.vColor = vin.vColor;
    
    float3 biasedPosL = vin.posL + (worldNormal * 0.02f);
    vout.posLight = mul(float4(biasedPosL, 1.f), mul(g_matWorld, g_matShadowLightViewProj));
    

    //vout.posLight = mul(float4(vin.posL, 1.f), mul(g_matWorld, g_matShadowLightViewProj));
    return vout;
}

// Pixel Shader
PS_OUT PSMain(VS_OUT pin)
{
    float4 albedo;
    if (pin.texID == 15)
    {
        albedo = gLavaStillTexture.Sample(gSamPointWrap, float2(pin.texCoord));
    }
    else if (pin.texID == 16)
    {
        albedo = gLavaFlowTexture.Sample(gSamPointWrap, float2(pin.texCoord));
    }
    else
    {
        albedo = gBlockTextureArray.Sample(gSamPointWrap, float3(pin.texCoord, pin.texID));
    }
    
    //clip(albedo.a - 0.5f);
    
    float3 finalColor;
    if(false)
    {
        //float3 N = normalize(pin.NormalW);
        //float3 L = normalize(-gDirLights.direction);
       // float NdotL = max(dot(N, L), 0.0f);
        //float3 ambient = 0.2f * albedo.rgb;
        //float3 diffuse = NdotL * gDirLights.diffuse.rgb * albedo.rgb;

        //finalColor = ambient + diffuse;
    }
    
    uint skyLight = (pin.light >> 4) & 0xf;
    uint blockLight = pin.light & 0xf;


    float sky = pow(skyLight / 15.f, 1.0f);
    float block = pow(blockLight / 15.f, 1.2f);

    //float sky = skyLight / 15.f;
    //float block = blockLight / 15.f;
    
    float dayFactor = g_fDayFactor;

    float finalLight =
    max(sky * dayFactor, block);

    finalColor =
    albedo.rgb * finalLight * pin.ao * pin.vColor.rgb;

    // 0: 그림자
    // 1: 빛 받음
    float fShadow = 0.f;
    {
        float3 vProj = pin.posLight.xyz / pin.posLight.w;
        float2 vShadowUV;
        vShadowUV.x = vProj.x * 0.5f + 0.5f;
        vShadowUV.y = -vProj.y * 0.5f + 0.5f;

        if (saturate(vShadowUV.x) == vShadowUV.x && saturate(vShadowUV.y) == vShadowUV.y)
        {
            float fCurrentDepth = vProj.z  ;
            fShadow = gShadowMap.SampleCmpLevelZero(gShadowSampler, vShadowUV, fCurrentDepth);
        }
        else // 범위 밖
        {
            fShadow = 1.f;
        }
    }

    // dayFactor 0.64에서 새도우맵을 안만듬
    // 자연스럽게 줄이기 위해  1.0 ~ 0.64 까지의 강도를 만듦
    float shadowIntensity = saturate((dayFactor - 0.64f) / (1.0f - 0.64f));
    
    // 최대 강도일때 그림자 감쇄량
    float targetMaxDarkness = 0.5f;
    
    // 그림자 강도가 0이면 즉, dayFactor가 0.64아래면 새도우는 없앤다
    // 그림자 강도가 > 0 이면 targetMaxDarkness 까지 보간함
    float minShadowAmbient = lerp(1.0f, targetMaxDarkness, shadowIntensity);
    
    // 블럭 라이팅 여부에따라 그림자 감쇄 지움
    float fFinalShadow = lerp(minShadowAmbient, 1.f, max(fShadow, block));

    // 스카이 라이트가 없는경우 그림자 감쇄량 무조건 지움
    if (skyLight == 0)
    {
        fFinalShadow = 1.0f;
    }
    
    // 마지막으로 그림자 감쇄량 곱해줌
    finalColor *= fFinalShadow;
    finalColor += albedo.rgb * 0.1f;
    
    PS_OUT pout;
    pout.target0 = float4(finalColor, albedo.a);
  
    return pout;
}