#include "../ShaderDefines.hlsl"


struct VS_IN
{
    float3 posL : POSITION;
};

struct VS_OUT
{
    float3 posW : POSITION;
};

struct GS_OUT
{
    float4 posH : SV_Position;
    uint light : LIGHT;
};

struct PS_OUT
{
    float4 target0 : SV_Target;
};

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT vout;
    float3 posW = g_vBlockPos + vin.posL * g_vExtents * 1.002f;
    vout.posW = posW;
    return vout;
}

[maxvertexcount(4)]
void GSMain(line VS_OUT input[2], inout TriangleStream<GS_OUT> stream)
{
    float3 p0 = input[0].posW;
    float3 p1 = input[1].posW;
    
    float3 lineDir = normalize(p1 - p0);
    
    float3 toCamera = normalize(g_vCamPos - (p0 + p1) * 0.5f);
    
    float3 right = normalize(cross(lineDir, toCamera)) * g_fThickness * 0.5f;
    
    float4x4 matVP = mul(g_matView, g_matProj);
    
    GS_OUT v;
    v.light = g_blockOutlineLight;
    
    v.posH = mul(float4(p0 - right, 1.f), matVP);
    stream.Append(v);
    v.posH = mul(float4(p0 + right, 1.f), matVP);
    stream.Append(v);
    v.posH = mul(float4(p1 - right, 1.f), matVP);
    stream.Append(v);
    v.posH = mul(float4(p1 + right, 1.f), matVP);
    stream.Append(v);
    
    
    stream.RestartStrip();

}

PS_OUT PSMain(GS_OUT pin)
{
   //uint blockLightRaw = pin.light & 0xF;
    //uint skyLightRaw = (pin.light >> 4) & 0xF;

    // 2. 0.0f ~ 1.0f 범위 비율 변환
   // float blockLight = blockLightRaw / 15.0f;
   // float skyLight = (skyLightRaw / 15.0f) * g_fDayFactor;

    // 3. 최종 라이트 강도 선택 및 암전 방지 보정
   // float finalLight = max(max(blockLight, skyLight), 0.08f);
    
    PS_OUT pout;
    //pout.target0 = g_vColor;
    pout.target0 = float4(g_vColor.rgb , g_vColor.a );
    return pout;
}