#include "../ShaderDefines.hlsl"


Texture2DArray gTexture32_32Array : register(t5);
SamplerState samp : register(s0);

struct VS_IN
{
    uint vertexID : SV_VertexID;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 viewDir : TEXCOORD0;
};

struct PS_OUT
{
    float4 target0 : SV_Target;
};

VS_OUT VSMain(VS_IN vin)
{
   
    
    VS_OUT output;

    float2 uv = float2((vin.vertexID << 1) & 2, vin.vertexID & 2);
    output.position = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 1.0f, 1.0f);
    
    output.position.z = output.position.w;

    float4 worldPos = mul(output.position, g_matInvViewProj);
    float3 rawWorldPos = worldPos.xyz / worldPos.w;

    output.viewDir = rawWorldPos - g_vCamPos;

    return output;
}
float Hash(float3 p)
{
    p = frac(p * 0.1031f);
    p += dot(p, p.yzx + 33.33f);
    return frac((p.x + p.y) * p.z);
}
PS_OUT PSMain(VS_OUT pin)
{
    float3 dir = normalize(pin.viewDir);
    float height = max(0.0f, dir.y);

    // 1. 기본 배경 그라데이션 계산
    float3 dayZenith = float3(0.2f, 0.4f, 0.8f);
    float3 dayHorizon = float3(0.6f, 0.7f, 0.9f);
    float3 daySky = lerp(dayHorizon, dayZenith, height);

    float3 nightZenith = float3(0.02f, 0.02f, 0.05f);
    float3 nightHorizon = float3(0.05f, 0.05f, 0.1f);
    float3 nightSky = lerp(nightHorizon, nightZenith, height);

    float3 finalColor = lerp(nightSky, daySky, g_fDayFactor);

    //  [수정] 해와 달과 마찬가지로, 하늘 역회전 행렬을 준비합니다.
    matrix matInvSkyRot = transpose(g_matSkyRotation);
    float3 localDir = mul(dir, (float3x3) matInvSkyRot);

    // -----------------------------------------------------------------
    //  밤하늘의 별 (Stars) 연산
    // -----------------------------------------------------------------
    if (g_fDayFactor < 0.5f)
    {
        //  셰이더 내부 연산 제거! C++에서 준 별 전용 행렬을 그대로 역행렬 취해 사용합니다.
        matrix matInvStarRot = transpose(g_matStarRotation);
        float3 starLocalDir = mul(dir, (float3x3) matInvStarRot);

        float3 starDir = starLocalDir * 120.0f;
        float3 box = floor(starDir);
        
        float noise = Hash(box);
        
        if (noise > 0.98f)
        {
            float3 center = box + 0.5f;
            float d = distance(starDir, center);
            
            float starMask = step(d, 0.3f);
            float3 starColor = float3(1.0f, 1.0f, 1.0f) * starMask * (1.0f - g_fDayFactor * 2.0f);
            
            finalColor += starColor;
        }
    }

    // -----------------------------------------------------------------
    //  [태양 렌더링] 및  [달 렌더링]
    // -----------------------------------------------------------------
    float spriteSize = 0.05f;

    if (localDir.y > 0.0f)
    {
        float2 planeUV = localDir.xz / localDir.y;
        if (abs(planeUV.x) < spriteSize && abs(planeUV.y) < spriteSize)
        {
            float2 sunUV = (planeUV / spriteSize) * 0.5f + 0.5f;
            float3 sunColor = gTexture32_32Array.Sample(samp, float3(sunUV, 0.0f)).rgb;
            
            //  [수정] g_fDayFactor를 곱하지 않고 태양 본연의 밝은 색을 그대로 덮어씁니다.
            finalColor = sunColor;
        }
    }
    
    if (localDir.y < 0.0f)
    {
        float2 planeUV = localDir.xz / -localDir.y;
        if (abs(planeUV.x) < spriteSize && abs(planeUV.y) < spriteSize)
        {
            float2 moonUV = (planeUV / spriteSize) * 0.5f + 0.5f;
            float3 moonColor = gTexture32_32Array.Sample(samp, float3(moonUV, 1.0f)).rgb;
            
            // 달은 낮에 사라져야 하므로 밤 가중치를 계속 유지합니다.
            finalColor = moonColor * (1.0f - g_fDayFactor);
        }
    }

    if (localDir.y < 0.0f)
    {
        float2 planeUV = localDir.xz / -localDir.y;
        if (abs(planeUV.x) < spriteSize && abs(planeUV.y) < spriteSize)
        {
            float2 moonUV = (planeUV / spriteSize) * 0.5f + 0.5f;
            float3 moonColor = gTexture32_32Array.Sample(samp, float3(moonUV, 1.0f)).rgb;
            finalColor = moonColor * (1.0f - g_fDayFactor);
        }
    }

    PS_OUT pout;
    pout.target0 = float4(finalColor, 1.0f);
    return pout;
}