#include "../ShaderDefines.hlsl"

static const float2 g_vScreenSize = float2(1280.f, 720.f);
static const float g_fGap = 0.f;
static const float g_fLength = 10.f;
static const float g_fCrosshairThickness = 2.f;
static const float g_fAlpha = 1.f;
static const float3 g_vCrosshairColor = float3(1.f, 1.f, 1.f);

struct VS_OUT
{
    float4 posH : SV_POSITION;
};
struct PS_IN
{
    float4 posH : SV_POSITION;
};
struct PS_OUT
{
    float4 target0 : SV_Target;
};

VS_OUT VSMain(uint id : SV_VertexID)
{
    VS_OUT vout;
    float2 uv = float2((id << 1) & 2, id & 2);
    vout.posH = float4(uv * 2.f - 1.f, 0.f, 1.f);
    return vout;
}

PS_OUT PSMain(PS_IN pin)
{
    PS_OUT pout;

    float2 center = g_vScreenSize * 0.5f;
    float2 uv = pin.posH.xy - center;

    float ax = abs(uv.x);
    float ay = abs(uv.y);
    float halfT = g_fCrosshairThickness * 0.5f;

    bool horiz = (ay <= halfT) && (ax >= g_fGap) && (ax <= g_fGap + g_fLength);
    bool vert = (ax <= halfT) && (ay >= g_fGap) && (ay <= g_fGap + g_fLength);

    if (!horiz && !vert)
        discard;

    pout.target0 = float4(g_vCrosshairColor, g_fAlpha);
    return pout;
}